#include <QtWidgets>

#include "configdialog.hpp"
#include "ui_configdialog.h"

#include <manager.hpp>
#include <utility.hpp>

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ConfigDialog) {
    ui->setupUi(this);

    settings = FDOSettings::getInstance();

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(handleOK()));
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked(bool)), this, SLOT(handleApply()));
    connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked(bool)), this, SLOT(handleClose()));
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked(bool)), this, SLOT(handleReset()));

    connect(ui->pkcsToolBinBrowse, SIGNAL(clicked(bool)), this, SLOT(handlePkcsToolBrowse()));
    connect(ui->pkcsEngineLibBrowse, SIGNAL(clicked(bool)), this, SLOT(handlePkcsEngineBrowse()));
    connect(ui->opensslBinBrowse, SIGNAL(clicked(bool)), this, SLOT(handleOpenSSLBrowse()));
    connect(ui->smartcardLibBrowse, SIGNAL(clicked(bool)), this, SLOT(handleSmartcardBrowse()));

    connect(ui->pkcsToolBinAuto, SIGNAL(clicked(bool)), this, SLOT(handlePkcsToolAuto()));
    connect(ui->pkcsEngineLibAuto, SIGNAL(clicked(bool)), this, SLOT(handlePkcsEngineAuto()));
    connect(ui->opensslBinAuto, SIGNAL(clicked(bool)), this, SLOT(handleOpenSSLAuto()));

    load();
}

ConfigDialog::~ConfigDialog() {
    delete ui;
}

void ConfigDialog::handleOK() {
    save();
    close();
}

void ConfigDialog::handleApply() {
    save();
}

void ConfigDialog::handleClose() {
    close();
}

void ConfigDialog::handleReset() {
    load();
}

void ConfigDialog::load() {
    SettingsManager::load();

    ui->pkcsToolBinValue->setText(settings->getPkcsToolBin());
    ui->pkcsEngineLibValue->setText(settings->getPkcsEngineLib());
    ui->opensslBinValue->setText(settings->getOpensslBin());

    ui->smartcardLibValue->setText(settings->getSmartcardLib());
}

void ConfigDialog::save() {
    settings->setPkcsToolBin(ui->pkcsToolBinValue->text());
    settings->setPkcsEngineLib(ui->pkcsEngineLibValue->text());
    settings->setOpensslBin(ui->opensslBinValue->text());

    settings->setSmartcardLib(ui->smartcardLibValue->text());

    SettingsManager::save();
}

void ConfigDialog::handlePkcsToolBrowse() {
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setOption(QFileDialog::ReadOnly, true);

    if (ui->pkcsToolBinValue->text().length() > 0)
        fileDialog.selectFile(ui->pkcsToolBinValue->text());

    int result = fileDialog.exec();
    if (result != QDialog::Accepted)
        return;

    QStringList selectedFiles = fileDialog.selectedFiles();
    if (selectedFiles.size() != 1)
        return;

    ui->pkcsToolBinValue->setText(selectedFiles[0]);
}

void ConfigDialog::handlePkcsEngineBrowse() {
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setOption(QFileDialog::ReadOnly, true);

    if (ui->pkcsEngineLibValue->text().length() > 0)
        fileDialog.selectFile(ui->pkcsEngineLibValue->text());

    int result = fileDialog.exec();
    if (result != QDialog::Accepted)
        return;

    QStringList selectedFiles = fileDialog.selectedFiles();
    if (selectedFiles.size() != 1)
        return;

    ui->pkcsEngineLibValue->setText(selectedFiles[0]);
}

void ConfigDialog::handleOpenSSLBrowse() {
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setOption(QFileDialog::ReadOnly, true);

    if (ui->opensslBinValue->text().length() > 0)
        fileDialog.selectFile(ui->opensslBinValue->text());

    int result = fileDialog.exec();
    if (result != QDialog::Accepted)
        return;

    QStringList selectedFiles = fileDialog.selectedFiles();
    if (selectedFiles.size() != 1)
        return;

    ui->opensslBinValue->setText(selectedFiles[0]);
}

void ConfigDialog::handleSmartcardBrowse() {
    QFileDialog fileDialog;
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setOption(QFileDialog::ReadOnly, true);

    if (ui->smartcardLibValue->text().length() > 0)
        fileDialog.selectFile(ui->smartcardLibValue->text());

    int result = fileDialog.exec();
    if (result != QDialog::Accepted)
        return;

    QStringList selectedFiles = fileDialog.selectedFiles();
    if (selectedFiles.size() != 1)
        return;

    ui->smartcardLibValue->setText(selectedFiles[0]);
}

void ConfigDialog::handlePkcsToolAuto() {
    ui->pkcsToolBinValue->setText(PathUtility::discoverPkcsToolBin());
}

void ConfigDialog::handlePkcsEngineAuto() {
    ui->pkcsEngineLibValue->setText(PathUtility::discoverPkcsEngineLib());
}

void ConfigDialog::handleOpenSSLAuto() {
    ui->opensslBinValue->setText(PathUtility::discoverOpenSSLBin());
}
