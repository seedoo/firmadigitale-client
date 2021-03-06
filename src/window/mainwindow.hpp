#ifndef __FDOTOOL_WINDOW_MAINWINDOW_H
#define __FDOTOOL_WINDOW_MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QLineEdit>

#include "fdosettings.hpp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

public slots:

    void updateSmartcardValues();

    void updateToolsValues();

private:
    Ui::MainWindow *ui;

    FDOSettings *settings;

    void updateLineEdit(QLineEdit *lineEdit, const QString &text = "", bool error = false);

    void initLineEdits();

    void doUpdateSmartcardValues();

signals:

    void showConfig();
};

#endif
