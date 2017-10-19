#ifndef __FDOTOOL_WINDOW_MAINWINDOW_H
#define __FDOTOOL_WINDOW_MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private:

    Ui::MainWindow *ui;

};

#endif
