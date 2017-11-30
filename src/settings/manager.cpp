#include <QtCore/QSettings>
#include <utility.hpp>

#include "manager.hpp"
#include "fdosettings.hpp"

void SettingsManager::load() {
    FDOSettings *fdoSettings = FDOSettings::getInstance();
    QSettings settings;

    settings.beginGroup("tools");
    fdoSettings->setPkcsToolBin(settings.value("pkcsToolBin", PathUtility::discoverPkcsToolBin()).toString());
    fdoSettings->setPkcsEngineLib(settings.value("pkcsEngineLib", PathUtility::discoverPkcsEngineLib()).toString());
    fdoSettings->setOpensslBin(settings.value("opensslBin", PathUtility::discoverOpenSSLBin()).toString());
    fdoSettings->setSmartcardLib(settings.value("smartcardLib", PathUtility::discoverSmartcardLib()).toString());
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
