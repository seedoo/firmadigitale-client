#include "processwindow.hpp"
#include "ui_processwindow.h"

#include <QtWidgets>
#include <QtSvg/QSvgRenderer>

ProcessWindow::ProcessWindow(QWidget *parent) : QDialog(parent), ui(new Ui::ProcessWindow) {
    ui->setupUi(this);

    svgWidget = new QSvgWidget(ui->iconWidget);
    ui->iconWidget->addWidget(svgWidget);

    updateAddress();
    updateUser();
    updateJobs();

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

void ProcessWindow::rpcError(QString title, QString message) {
    QMessageBox::critical(this, title, message);

    emit waitAndClose();
}

void ProcessWindow::updateProgress(int value, int max) {
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(value);
}

void ProcessWindow::updateStep(QString text) {
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString line = QString("<strong>%1</strong> - %2\n").arg(dateTime).arg(text);
    ui->logText->append(line);
    ui->logText->moveCursor(QTextCursor::End);
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

void ProcessWindow::updateJobs(int jobs) {
    if (jobs < 0) {
        ui->jobsValue->setStyleSheet("color: gray;");
        ui->jobsValue->setText("loading...");
    } else {
        ui->jobsValue->setStyleSheet("color: black;");
        ui->jobsValue->setText(QString::number(jobs));
    }
}

void ProcessWindow::iconWorking() {
    svgWidget->load(QString(":/status/loading"));
}

void ProcessWindow::iconCompleted() {
    svgWidget->load(QString(":/status/completed"));
}
