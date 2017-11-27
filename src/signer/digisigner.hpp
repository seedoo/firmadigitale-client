#ifndef __FDOTOOL_SIGNER_DIGISIGNER_H
#define __FDOTOOL_SIGNER_DIGISIGNER_H

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QDir>

class DigiSigner : public QObject {
Q_OBJECT

public:

    explicit DigiSigner(QObject *parent = Q_NULLPTR);

    bool initCard();

    void setPin(const QString &pin);

    QByteArray cadesSign(const QByteArray &inputData);

private:
    static QDir installDir;

    QString signerPKCS11Engine;
    QString certificateId;
    QString certificate;
    QString pin;

    static QString getCertId();

    static QString getCert(QString id);

    static void waitForString(QProcess &process, const QString &text);

    static QString locatePKCS11EngineLibrary();

signals:

    void progressStep(QString message);

    void error(QString message);
};

#endif
