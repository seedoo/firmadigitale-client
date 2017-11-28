#include <QtCore/QSettings>

#include "manager.hpp"
#include "fdosettings.hpp"
#include "defaults.hpp"

void SettingsManager::load() {
    FDOSettings *fdoSettings = FDOSettings::getInstance();
    QSettings settings;

    settings.beginGroup("tools");
    fdoSettings->setPkcsToolBin(settings.value("pkcsToolBin", SETTINGS_PKCSTOOL_BIN_DEFAULT).toString());
    fdoSettings->setPkcsEngineLib(settings.value("pkcsEngineLib", SETTINGS_PKCSENGINE_LIB_DEFAULT).toString());
    fdoSettings->setOpensslBin(settings.value("opensslBin", SETTINGS_OPENSSL_BIN_DEFAULT).toString());
    fdoSettings->setSmartcardLib(settings.value("smartcardLib", SETTINGS_SMARTCARD_LIB_DEFAULT).toString());
    settings.endGroup();
}

void SettingsManager::save() {
    FDOSettings *config = FDOSettings::getInstance();
    QSettings settings;

    settings.beginGroup("tools");
    settings.setValue("pkcsToolBin", config->getPkcsToolBin());
    settings.setValue("pkcsEngineLib", config->getPkcsEngineLib());
    settings.setValue("opensslBin", config->getOpensslBin());
    settings.setValue("smartcardLib", config->getSmartcardLib());
    settings.endGroup();

    settings.sync();
}
