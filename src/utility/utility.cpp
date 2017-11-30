#include <config.hpp>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include "utility.hpp"

QString PathUtility::discoverPkcsToolBin() {
    QStringList searchPath;

#ifdef Q_OS_WIN32
    searchPath << "C:\\Program Files (x86)\\OpenSC Project\\OpenSC\\tools\\pkcs11-tool.exe";
    searchPath << "C:\\Program Files\\OpenSC Project\\OpenSC\\tools\\pkcs11-tool.exe";
#else
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
        if (line.contains(FDOTOOL_TOOLS_PKCSTOOL_BIN_NAME))
            searchPath.append(line.trimmed());
#endif

    for (const QString &path: searchPath)
        if (isValid(path, FDOTOOL_TOOLS_PKCSTOOL_BIN_NAME))
            return path;

    return "";
}

QString PathUtility::discoverPkcsEngineLib() {
    QStringList searchPath;

#ifdef Q_OS_WIN32
    searchPath << "C:\\Windows\\SysWOW64\\opensc-pkcs11.dll";
    searchPath << "C:\\Windows\\System32\\opensc-pkcs11.dll";
#elif Q_OS_MAC
    searchPath << "opensc-pkcs11.dylib";
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
        if (line.contains(FDOTOOL_TOOLS_PKCSENGINE_LIB_NAME))
            searchPath.append(line.trimmed());
#endif

    for (const QString &path: searchPath)
        if (isValid(path, FDOTOOL_TOOLS_PKCSENGINE_LIB_NAME))
            return path;

    return "";
}

QString PathUtility::discoverOpenSSLBin() {
    QStringList searchPath;

#ifdef Q_OS_WIN32
    searchPath << "C:\\Windows\\SysWOW64\\opensc-pkcs11.dll";
    searchPath << "C:\\Windows\\System32\\opensc-pkcs11.dll";
#elif Q_OS_MAC
    searchPath << "opensc-pkcs11.dylib";
#else
    QDir installDir(FDOTOOL_TOOLS_INSTALL_DIR);
    searchPath.append(installDir.absoluteFilePath(FDOTOOL_TOOLS_OPENSSL_BIN_NAME));
#endif

    for (const QString &path: searchPath)
        if (isValid(path, FDOTOOL_TOOLS_OPENSSL_BIN_NAME))
            return path;

    return "";
}

QString PathUtility::discoverSmartcardLib() {
    QStringList searchPath;

#ifdef Q_OS_WIN32
    searchPath.append(installDir.absoluteFilePath(FDOTOOL_OPENSSL_BINARY));
#elif Q_OS_MAC
    searchPath.append(installDir.absoluteFilePath(FDOTOOL_OPENSSL_BINARY));
#else
    QDir installDir(FDOTOOL_TOOLS_INSTALL_DIR);
    searchPath.append(installDir.absoluteFilePath(FDOTOOL_TOOLS_SMARTCARD_LIB_NAME));
#endif

    for (const QString &path: searchPath)
        if (isValid(path, FDOTOOL_TOOLS_SMARTCARD_LIB_NAME))
            return path;

    return "";
}

bool PathUtility::isValid(const QString &path, const QString &name) {
    if (path.length() == 0)
        return false;

    QFileInfo fileInfo(path);

    if (!fileInfo.exists() || !fileInfo.isFile())
        return false;

    if (name.length() > 0) {
        QString libName = name;
        if (!name.startsWith("lib"))
            libName = QString("lib%1").arg(name);

        if (fileInfo.fileName() != name
            && fileInfo.fileName() != libName)
            return false;
    }

    return true;
}
