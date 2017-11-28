#ifndef __FDOTOOL_UTILITY_UTILITY_H
#define __FDOTOOL_UTILITY_UTILITY_H

#include <QtCore/QString>
#include <QtCore/QDir>

class PathUtility {

public:

    static QString discoverPkcsToolBin();

    static QString discoverOpenSSLBin();

    static QString discoverPkcsEngineLib();

private:
    static QDir installDir;
};

#endif
