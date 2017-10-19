#include <QEventLoop>
#include <QNetworkReply>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

#include "odooworker.hpp"

OdooWorker::OdooWorker(QObject *parent) : QObject(parent) {

}

OdooWorker::~OdooWorker() {

}

void OdooWorker::doAction(Action action) {
    odooUrl = action.getOdooUrl();
    token = action.getToken();

    emit updateProgress(0, FDOTOOL_ODOOWORKER_PROGRESS_MAX);
    emit updateAddress(odooUrl);
    emit updateUser("");

    QVariantMap request;
    request.insert("token", token);
    QVariantMap response = call(FDOTOOL_ODOOWORKER_API_BOOTSTRAP, request);

    emit updateUser(response["user"].toString());
}

QVariantMap OdooWorker::call(QString api, QVariantMap data) {
    QString url = QString("%1%2").arg(odooUrl).arg(api);

    QJsonObject requestBody;
    requestBody.insert("jsonrpc", "2.0");
    requestBody.insert("method", "call");
    requestBody.insert("id", QJsonValue::Null);
    requestBody.insert("params", QJsonValue::fromVariant(data));

    QByteArray rawResponse = doPost(QUrl(url), QJsonDocument(requestBody).toJson(QJsonDocument::Compact));

    QJsonDocument responseDocument = QJsonDocument::fromJson(rawResponse);
    QJsonObject responseBody = responseDocument.object();
    QJsonObject result = responseBody["result"].toObject();

    return result.toVariantMap();
};

QByteArray OdooWorker::doPost(QUrl url, QByteArray requestBody) {
    QEventLoop eventLoop;
    connect(&qNetworkAccessManager, SIGNAL(finished(QNetworkReply * )), &eventLoop, SLOT(quit()));

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);

    QNetworkReply *reply = qNetworkAccessManager.post(request, requestBody);
    eventLoop.exec();

    reply->deleteLater();
    return reply->readAll();
}