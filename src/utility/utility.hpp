#ifndef __FDOTOOL_UTILITY_UTILITY_H
#define __FDOTOOL_UTILITY_UTILITY_H

#include <QtCore/QString>
#include <QtCore/QDir>

#ifdef Q_OS_WIN32
#define FDOTOOL_TOOLS_PKCSTOOL_BIN_NAME "pkcs11-tool.exe"
#define FDOTOOL_TOOLS_PKCSENGINE_LIB_NAME "opensc-pkcs11.dll"
#define FDOTOOL_TOOLS_OPENSSL_BIN_NAME "openssl.exe"
#define FDOTOOL_TOOLS_SMARTCARD_LIB_NAME "bit4xpki.dll"
#elif Q_OS_MAC
#define FDOTOOL_TOOLS_PKCSTOOL_BIN_NAME "pkcs11-tool"
#define FDOTOOL_TOOLS_PKCSENGINE_LIB_NAME "opensc-pkcs11.so"
#define FDOTOOL_TOOLS_OPENSSL_BIN_NAME "openssl"
#define FDOTOOL_TOOLS_SMARTCARD_LIB_NAME "bit4xpki.dll"
#else
#define FDOTOOL_TOOLS_PKCSTOOL_BIN_NAME "pkcs11-tool"
#define FDOTOOL_TOOLS_PKCSENGINE_LIB_NAME "pkcs11.so"
#define FDOTOOL_TOOLS_OPENSSL_BIN_NAME "openssl"
#define FDOTOOL_TOOLS_SMARTCARD_LIB_NAME "libbit4xpki.so"
#endif

class PathUtility {

public:

    static QString discoverPkcsToolBin();

    static QString discoverOpenSSLBin();

    static QString discoverPkcsEngineLib();

    static QString discoverSmartcardLib();

private:

    static bool isValid(const QString &path = "", const QString &name = "");
};

#endif
