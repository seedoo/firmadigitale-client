#include <iostream>
#include <QtCore/QProcess>
#include <QtNetwork/QSslCertificate>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>

#include "config.hpp"

#include "digisigner.hpp"

QDir DigiSigner::installDir(FDOTOOL_INSTALL_PREFIX);

DigiSigner::DigiSigner(QObject *parent) : QObject(parent) {
}

bool DigiSigner::initCard() {
    signerPKCS11Engine = locatePKCS11EngineLibrary();
    if (signerPKCS11Engine.length() == 0) {
        emit error("PKCS#11 Engine Library not found");
        return false;
    }

    certificateId = getCertId();
    if (certificateId.length() == 0) {
        emit error("Smart card not found");
        return false;
    }

    certificate = getCert(certificateId);

    return true;
}

void DigiSigner::setPin(const QString &pin) {
    this->pin = pin;
}

QByteArray DigiSigner::cadesSign(const QByteArray &inputData) {
    if (certificateId.length() == 0
        || certificate.length() == 0
        || pin.length() == 0) {
        emit error("Not initialized or pin not set");
        return QByteArray();
    }

    QString tempFilePrefix = QDir(QDir::tempPath()).absoluteFilePath("tmpdigisigner");

    QTemporaryFile crtFile(tempFilePrefix);
    crtFile.setAutoRemove(false);
    crtFile.open();
    crtFile.write(certificate.toLatin1());
    crtFile.close();

    QTemporaryFile inFile(tempFilePrefix);
    inFile.setAutoRemove(false);
    inFile.open();
    inFile.write(inputData);
    inFile.close();

    QTemporaryFile outFile(tempFilePrefix);
    outFile.setAutoRemove(false);
    outFile.open();
    outFile.close();

    QString engineName = "pkcs11";

    QStringList engineArgs;
    engineArgs << "engine";
    engineArgs << "dynamic";
    engineArgs << "-pre" << QString("SO_PATH:%1").arg(signerPKCS11Engine);
    engineArgs << "-pre" << QString("ID:%1").arg(engineName);
    engineArgs << "-pre" << "LIST_ADD:1";
    engineArgs << "-pre" << "LOAD";
    engineArgs << "-pre" << QString("MODULE_PATH:%1").arg(installDir.absoluteFilePath(SIGNER_BIT4X_PKI_LIBRARY));
    engineArgs << "-pre" << QString("PIN:%1").arg(pin);

    QStringList smimeArgs;
    smimeArgs << "smime";
    smimeArgs << "-md" << "sha256";
    smimeArgs << "-nodetach";
    smimeArgs << "-binary";
    smimeArgs << "-outform" << "DER";
    smimeArgs << "-sign";
    smimeArgs << "-signer" << crtFile.fileName();
    smimeArgs << "-inkey" << QString("id_%1").arg(certificateId);
    smimeArgs << "-keyform" << "engine";
    smimeArgs << "-engine" << engineName;
    smimeArgs << "-in" << inFile.fileName();
    smimeArgs << "-out" << outFile.fileName();

    QString engineCmd = engineArgs.join(" ") + "\n";
    QString smimeCmd = smimeArgs.join(" ") + "\n";

    emit progressStep("Starting OpenSSL");
    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.start(installDir.absoluteFilePath(SIGNER_OPENSSL_BINARY));
    process.waitForStarted();

    waitForString(process, "OpenSSL>");

    emit progressStep("Starting engine");
    process.write(engineCmd.toLatin1());
    waitForString(process, "OpenSSL>");

    emit progressStep("Signing file");
    process.write(smimeCmd.toLatin1());
    waitForString(process, "OpenSSL>");

    QByteArray stdOut = process.readAllStandardOutput();
    QByteArray stdErr = process.readAllStandardError();

    process.terminate();
    process.waitForFinished();

    if (stdErr.contains("Login failed")) {
        process.readAll();
        emit error("PIN NOT VALID");
        return QByteArray();
    }

    emit progressStep("Reading output");
    outFile.open();
    QByteArray outputData = outFile.readAll();
    outFile.close();

    crtFile.remove();
    inFile.remove();
    outFile.remove();

    return outputData;
}

QString DigiSigner::getCertId() {
    QStringList arguments;
    arguments << "--module" << installDir.absoluteFilePath(SIGNER_BIT4X_PKI_LIBRARY);
    arguments << "--list-objects";
    arguments << "--type" << "cert";

    QProcess process;
    process.start(installDir.absoluteFilePath(SIGNER_PKCS11_TOOL_BINARY), arguments);
    process.waitForStarted();
    process.waitForFinished();
    if (process.exitStatus() != QProcess::NormalExit)
        return "";

    QString rawOutput(process.readAll());
    QStringList lines = rawOutput.split('\n');

    bool containsCertificate = false;

    for (const QString &line: lines) {

        if (line.startsWith("Certificate Object") && line.contains("type = X.509 cert"))
            containsCertificate = true;
        else if (!line.startsWith("  "))
            containsCertificate = false;

        if (containsCertificate && line.startsWith("  ID:")) {
            QStringList items = line.trimmed().split(':', QString::SkipEmptyParts);

            if (items.length() == 2) {
                QString certificateId = items[1].trimmed();
                return certificateId;
            }
        }
    }

    return "";
}

QString DigiSigner::getCert(QString id) {
    QString cert = "";

    QStringList arguments;
    arguments << "--module" << installDir.absoluteFilePath(SIGNER_BIT4X_PKI_LIBRARY);
    arguments << "--read-object";
    arguments << "--type" << "cert";
    arguments << "--id" << id;

    QProcess process;
    process.start(installDir.absoluteFilePath(SIGNER_PKCS11_TOOL_BINARY), arguments);
    process.waitForStarted();
    process.waitForFinished();
    if (process.exitStatus() != QProcess::NormalExit)
        return cert;

    QByteArray certBytes = process.readAll();

    QSslCertificate certificate(certBytes, QSsl::Der);
    cert = QString(certificate.toPem());

    return cert;
}

void DigiSigner::waitForString(QProcess &process, const QString &text) {
    while (true) {
        process.waitForReadyRead(250);

        QByteArray data = process.readLine();
        if (data.startsWith(text.toLatin1())) {
            process.readAll();
            break;
        }
    }
}

QString DigiSigner::locatePKCS11EngineLibrary() {
#ifdef Q_OS_WIN32
    return "pkcs11.dll";
#else
    QStringList args;
    args << "-L" << "libengine-pkcs11-openssl";

    QProcess process;
    process.start("dpkg", args);
    process.waitForStarted();
    process.waitForFinished();
    if (process.exitStatus() != QProcess::NormalExit)
        return "";

    QString rawOutput(process.readAll());
    QStringList lines = rawOutput.split('\n');

    for (const QString &line: lines)
        if (line.contains("libpkcs11.so"))
            return line;

    return "";
#endif
}
