#ifndef __FDOTOOL_WINDOW_PROCESSWINDOW_H
#define __FDOTOOL_WINDOW_PROCESSWINDOW_H

#include <QDialog>

namespace Ui {
    class ProcessWindow;
}

class ProcessWindow : public QDialog {
Q_OBJECT

public:
    explicit ProcessWindow(QWidget *parent = nullptr);

    ~ProcessWindow();

public slots:

    void updateAddress(QString address);

    void updateUser(QString user);

    void updateProgress(int value, int max);

private:

    Ui::ProcessWindow *ui;

    void signalConnect();

private slots:

    void handleClose();
};

#endif
