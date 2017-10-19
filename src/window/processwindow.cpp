#include "processwindow.hpp"
#include "ui_processwindow.h"

#include <QtWidgets>

ProcessWindow::ProcessWindow(QWidget *parent) : QDialog(parent), ui(new Ui::ProcessWindow) {
    ui->setupUi(this);

    signalConnect();
}

ProcessWindow::~ProcessWindow() {
    delete ui;
}

void ProcessWindow::signalConnect() {
    connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked(bool)), this, SLOT(handleClose()));
}

void ProcessWindow::handleClose() {
    close();
}

void ProcessWindow::updateAddress(QString address) {
    if (address.length() == 0) {
        ui->addressValue->setStyleSheet("color: gray;");
        ui->addressValue->setText("loading...");
    } else {
        ui->addressValue->setStyleSheet("color: black;");
        ui->addressValue->setText(address);
    }
}

void ProcessWindow::updateUser(QString user) {
    if (user.length() == 0) {
        ui->userValue->setStyleSheet("color: gray;");
        ui->userValue->setText("loading...");
    } else {
        ui->userValue->setStyleSheet("color: black;");
        ui->userValue->setText(user);
    }
}

void ProcessWindow::updateProgress(int value, int max) {
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(value);
}
