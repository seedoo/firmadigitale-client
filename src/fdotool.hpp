#ifndef __FDOTOOL_FDOTOOL_H
#define __FDOTOOL_FDOTOOL_H

#include <QApplication>
#include <QList>
#include <QMessageLogger>

#include "version.hpp"
#include "action.hpp"

#include "worker/odooworker.hpp"

#include "window/mainwindow.hpp"
#include "window/processwindow.hpp"

enum FDOMode {

    MAIN,
    ODOO
};

class FDOTool : public QApplication {
Q_OBJECT

public:

    explicit FDOTool(int &argc, char **argv);

    ~FDOTool();

    void parseCommandLine();

    int run();

private:
    FDOMode mode;
    QList<Action> actions;

    OdooWorker *odooWorker;

    MainWindow *mainWindow;
    ProcessWindow *processWindow;

    void doWaitAndClose(bool wait = false);

    void doMain();

    void doOdoo();

private slots:

    void waitAndClose();

    void showConfig();

};

#endif
