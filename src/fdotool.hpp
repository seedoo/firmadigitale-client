#ifndef __FDOTOOL_FDOTOOL_H
#define __FDOTOOL_FDOTOOL_H

#include <QApplication>
#include <QList>
#include <QMessageLogger>

#include "version.hpp"
#include "action.hpp"

class FDOTool : public QApplication {
Q_OBJECT

public:

    explicit FDOTool(int &argc, char **argv);

    void parseCommandLine();

    int run();

private:
    QList<Action> actions;
};

#endif
