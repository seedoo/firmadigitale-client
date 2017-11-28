#include <QtCore/QDir>
#include <digisigner.hpp>
#include <QtConcurrent/QtConcurrent>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    settings = FDOSettings::getInstance();

    connect(ui->actionFileExit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionConfigEdit, SIGNAL(triggered(bool)), this, SIGNAL(showConfig()));
    connect(ui->actionConfigReload, SIGNAL(triggered(bool)), this, SLOT(updateSmartcardValues()));

    initLineEdits();
    updateToolsValues();
    updateSmartcardValues();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateLineEdit(QLineEdit *lineEdit, const QString &text, bool error) {
    QString align = "left";
    QString color = "black";
    QString content = text;

    if (error)
        color = "red";

    if (text.length() == 0) {
        color = "grey";
        content = "loading...";
    }

    lineEdit->setStyleSheet(QString("color: %1; text-align: %2;").arg(color).arg(align));
    lineEdit->setText(content);
}

void MainWindow::initLineEdits() {
    updateLineEdit(ui->pkcsToolValue);
    updateLineEdit(ui->pkcsEngineValue);
    updateLineEdit(ui->opensslExecValue);
    updateLineEdit(ui->smartcardLibValue);

    updateLineEdit(ui->certIdValue);
}

void MainWindow::updateToolsValues() {
    updateLineEdit(ui->pkcsToolValue, settings->getPkcsToolBin());
    updateLineEdit(ui->pkcsEngineValue, settings->getPkcsEngineLib());
    updateLineEdit(ui->opensslExecValue, settings->getOpensslBin());
    updateLineEdit(ui->smartcardLibValue, settings->getSmartcardLib());
}

void MainWindow::updateSmartcardValues() {
    updateLineEdit(ui->certIdValue);
    QtConcurrent::run(this, &MainWindow::doUpdateSmartcardValues);
}

void MainWindow::doUpdateSmartcardValues() {
    QString certId = DigiSigner::getCertId();

    if (certId.length() > 0) {
        updateLineEdit(ui->certIdValue, certId);
    } else {
        updateLineEdit(ui->certIdValue, "Smart Card not found", true);
    }
}
