#ifndef __FDOTOOL_WORKER_ODOOWORKER_H
#define __FDOTOOL_WORKER_ODOOWORKER_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>

#include "../action.hpp"

#define FDOTOOL_ODOOWORKER_PROGRESS_MAX 10

#define FDOTOOL_ODOOWORKER_API_BOOTSTRAP "/fdo/1/action/bootstrap"

class OdooWorker : public QObject {
Q_OBJECT

public:

    explicit OdooWorker(QObject *parent = Q_NULLPTR);

    ~OdooWorker();

    void doAction(Action action);

private:
    QString odooUrl;
    QString token;

    QNetworkAccessManager qNetworkAccessManager;

    QVariantMap call(QString api, QVariantMap data);

    QByteArray doPost(QUrl url, QByteArray requestBody);

signals:

    void updateAddress(QString address);

    void updateUser(QString user);

    void updateProgress(int value, int max);
};

#endif
