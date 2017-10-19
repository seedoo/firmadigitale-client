#ifndef __FDOTOOL_WORKER_ODOOWORKER_H
#define __FDOTOOL_WORKER_ODOOWORKER_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkAccessManager>

#include "../action.hpp"

class OdooWorker : public QObject {
Q_OBJECT

public:

    explicit OdooWorker(QObject *parent = Q_NULLPTR);

    ~OdooWorker();

    void doAction(Action action);

private:
    QNetworkAccessManager qNetworkAccessManager;

    QString doPost(QUrl url, QString requestBody);

signals:

    void updateAddress(QString address);

    void updateUser(QString user);

    void updateProgress(int value, int max);
};

#endif
