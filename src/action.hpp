#ifndef __FDOTOOL_ACTION_H
#define __FDOTOOL_ACTION_H

#include "version.hpp"

#include <QString>

class Action {

public:

    Action();

    Action(const QString &string, const QString &qString);

    const QString &getOdooUrl() const;

    void setOdooUrl(const QString &odooUrl);

    const QString &getToken() const;

    void setToken(const QString &token);

private:
    QString odooUrl;
    QString token;
};

#endif
