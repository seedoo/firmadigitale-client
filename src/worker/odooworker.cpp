#include <QEventLoop>
#include <QNetworkReply>

#include "odooworker.hpp"

OdooWorker::OdooWorker(QObject *parent) : QObject(parent) {

}

OdooWorker::~OdooWorker() {

}

void OdooWorker::doAction(Action action) {
    int max = 10;

    emit updateProgress(0, max);
    emit updateAddress(action.getOdooUrl());
    emit updateUser("");

}

QString OdooWorker::doPost(QUrl url, QString requestBody) {
    QEventLoop eventLoop;
    connect(&qNetworkAccessManager, SIGNAL(finished(QNetworkReply * )), &eventLoop, SLOT(quit()));

    QNetworkRequest request;
    request.setUrl(url);

    QNetworkReply *reply = qNetworkAccessManager.post(request, requestBody.toLatin1());
    eventLoop.exec();

    reply->deleteLater();
    QByteArray rawData = reply->readAll();

    return QString(rawData);
}