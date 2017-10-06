#include "action.hpp"

Action::Action() = default;

Action::Action(const QString &odooUrl, const QString &token) {
    this->odooUrl = odooUrl;
    this->token = token;
}

const QString &Action::getOdooUrl() const {
    return odooUrl;
}

void Action::setOdooUrl(const QString &odooUrl) {
    this->token = odooUrl;
}

const QString &Action::getToken() const {
    return token;
}

void Action::setToken(const QString &token) {
    this->token = token;
}
