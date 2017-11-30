#ifndef __FDOTOOL_WINDOW_CONFIGDIALOG_H
#define __FDOTOOL_WINDOW_CONFIGDIALOG_H

#include <QDialog>
#include <fdosettings.hpp>

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog {
Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);

    ~ConfigDialog();

private:
    Ui::ConfigDialog *ui;

    FDOSettings *settings;

    void load();

    void save();

private slots:

    void handleOK();

    void handleApply();

    void handleClose();

    void handleReset();

    void handlePkcsToolBrowse();

    void handlePkcsEngineBrowse();

    void handleOpenSSLBrowse();

    void handleSmartcardBrowse();

    void handlePkcsToolAuto();

    void handlePkcsEngineAuto();

    void handleOpenSSLAuto();

    void handleSmartcardAuto();
};

#endif
