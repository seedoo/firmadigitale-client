#include <config.hpp>
#include <QtCore/QProcess>

#include "utility.hpp"
#include "fdosettings.hpp"

QDir PathUtility::installDir(FDOTOOL_INSTALL_PREFIX);

QString PathUtility::discoverPkcsToolBin() {
#ifdef Q_OS_LINUX
    QStringList args;
    args << "pkcs11-tool";

    QProcess process;
    process.start("which", args);
    process.waitForStarted();
    process.waitForFinished();
    if (process.exitStatus() != QProcess::NormalExit)
        return "";

    QString rawOutput(process.readAll());
    QStringList lines = rawOutput.split('\n');

    for (const QString &line: lines)
        if (line.contains("pkcs11-tool"))
            return line.trimmed();

    return "";
#else
    return installDir.absoluteFilePath("pkcs11-tool.exe");
#endif
}

QString PathUtility::discoverPkcsEngineLib() {
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

QString PathUtility::discoverOpenSSLBin() {
    return installDir.absoluteFilePath(FDOTOOL_OPENSSL_BINARY);
}
