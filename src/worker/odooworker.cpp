#include <QEventLoop>
#include <QNetworkReply>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QThread>
#include <QtCore/QJsonArray>

#include "odooworker.hpp"
#include "digisigner.hpp"

OdooWorker::OdooWorker(QObject *parent) : QObject(parent) {

}

OdooWorker::~OdooWorker() {

}

bool OdooWorker::doAction(Action action) {
    QVariantMap request;
    QVariantMap response;

    int progress = 0;
    int progressMax = FDOTOOL_ODOOWORKER_PROGRESS_MIN_STEPS;

    emit updateProgress(progress, progressMax);
    emit updateStep("Starting action");

    QThread::msleep(500);

    progress++;
    emit updateProgress(progress, progressMax);
    emit updateStep("Getting URL");

    odooUrl = action.getOdooUrl();
    token = action.getToken();
    emit updateAddress(odooUrl);

    QThread::msleep(500);

    progress++;
    emit updateProgress(progress, progressMax);
    emit updateStep("Requesting session bootstrap to Odoo");

    request.clear();
    request.insert("token", token);
    response = jsonRpc(FDOTOOL_ODOOWORKER_API_BOOTSTRAP, request);
    if (response.empty())
        return false;

    if (!response.contains("userName")
        || !response.contains("jobsIds")) {
        emit rpcError("RPC Error", "Response not valid");
        return false;
    }

    progress++;
    emit updateProgress(progress, progressMax);
    emit updateStep("Getting user and jobs for session");

    QString user = response["userName"].toString();
    emit updateUser(user);

    QVariantList jobsList = response["jobsIds"].toList();
    int jobs = jobsList.size();
    emit updateJobs(jobs);

    progressMax = FDOTOOL_ODOOWORKER_PROGRESS_MIN_STEPS + (jobs * 5);

    for (const auto &job : jobsList) {
        qulonglong jobId = job.toULongLong();

        progress++;
        emit updateProgress(progress, progressMax);
        emit updateStep(QString("Getting job %1").arg(jobId));

        request.clear();
        request.insert("token", token);
        request.insert("jobId", jobId);
        response = jsonRpc(FDOTOOL_ODOOWORKER_API_GETJOB, request);
        if (response.empty())
            return false;

        if (!response.contains("action")
            || !response.contains("attachmentId")) {
            emit rpcError("RPC Error", "Response not valid");
            return false;
        }

        QString actionToDo = response["action"].toString();
        qulonglong attachmentId = response["attachmentId"].toULongLong();

        emit updateStep(QString("Action to do: %1").arg(actionToDo));

        progress++;
        emit updateProgress(progress, progressMax);
        emit updateStep(QString("Getting attachment %1").arg(attachmentId));

        request.clear();
        request.insert("token", token);
        request.insert("jobId", jobId);
        request.insert("attachmentId", attachmentId);
        response = jsonRpc(FDOTOOL_ODOOWORKER_API_GETATTACHMENT, request);
        if (response.empty())
            return false;

        if (!response.contains("content")) {
            emit rpcError("RPC Error", "Response not valid");
            return false;
        }

        QString base64Content = response["content"].toString();
        QByteArray content = QByteArray::fromBase64(base64Content.toUtf8());

        progress++;
        emit updateProgress(progress, progressMax);
        emit updateStep(QString("Attachment %1 is %2 bytes").arg(jobId).arg(content.length()));

        if (actionToDo == "sign") {
            emit updateStep(QString("Signing data").arg(jobId).arg(content.length()));
            DigiSigner digiSigner(content);
            digiSigner.sign();

            QByteArray signedContent = digiSigner.getOutputData();

            emit updateStep(QString("Signed file is %1 bytes").arg(signedContent.length()));

            emit updateStep("Uploading result to Odoo");

            request.clear();
            request.insert("token", token);
            request.insert("jobId", jobId);
            request.insert("attachmentId", attachmentId);
            request.insert("signedContent", QString(signedContent.toBase64()));
            response = jsonRpc(FDOTOOL_ODOOWORKER_API_UPLOADSIGNED, request);
            if (response.empty())
                return false;

            emit updateStep("Upload complete");
        }
    }

    return true;
}

QVariantMap OdooWorker::jsonRpc(const QString &api, const QVariantMap &data) {
    QString url = QString("%1%2").arg(odooUrl).arg(api);

    QJsonObject requestBody;
    requestBody.insert("jsonrpc", "2.0");
    requestBody.insert("method", "call");
    requestBody.insert("id", QJsonValue::Null);
    requestBody.insert("params", QJsonValue::fromVariant(data));

    QByteArray rawResponse = doPost(QUrl(url), QJsonDocument(requestBody).toJson(QJsonDocument::Compact));

    if (rawResponse.length() == 0) {
        emit rpcError("RPC Error", "Empty response");
        return QVariantMap();
    }

    QJsonDocument responseDocument = QJsonDocument::fromJson(rawResponse);
    QJsonObject responseBody = responseDocument.object();
    if (responseBody.empty() || !responseBody.contains("result")) {
        emit rpcError("RPC Error", "Not a valid RPC response");
        return QVariantMap();
    }

    QJsonObject result = responseBody["result"].toObject();
    if (result.empty() || !result.contains("success")) {
        emit rpcError("RPC Error", "Invalid result");
        return QVariantMap();
    }

    bool success = result.value("success").toBool();
    if (!success) {
        QString message = result.value("error").toString();
        emit rpcError("RPC Error", message);
        return QVariantMap();
    }

    return result.toVariantMap();
};

QByteArray OdooWorker::doPost(const QUrl &url, const QByteArray &requestBody) {
    QEventLoop eventLoop;
    connect(&qNetworkAccessManager, SIGNAL(finished(QNetworkReply * )), &eventLoop, SLOT(quit()));

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);

    QNetworkReply *reply = qNetworkAccessManager.post(request, requestBody);
    eventLoop.exec();

    reply->deleteLater();

    QNetworkReply::NetworkError networkError = reply->error();
    if (networkError != QNetworkReply::NoError) {
        QString errorString = reply->errorString();
        emit rpcError("RPC Error", errorString);

        return QByteArray();
    }

    QByteArray responseData = reply->readAll();

    return responseData;
}
