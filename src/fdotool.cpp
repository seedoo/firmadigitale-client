#include <QtCore/QCommandLineParser>
#include <QtCore/QUrlQuery>
#include <QtConcurrent/QtConcurrent>

#include "fdotool.hpp"

int main(int argc, char *argv[]) {
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);

    FDOTool fdoTool(argc, argv);
    fdoTool.parseCommandLine();
    return fdoTool.run();
}

FDOTool::FDOTool(int &argc, char **argv) : QApplication(argc, argv) {
    odooWorker = new OdooWorker();

    mainWindow = new MainWindow();
    processWindow = new ProcessWindow();

    mode = MAIN;

    actions.clear();
}

FDOTool::~FDOTool() {
    delete odooWorker;

    delete mainWindow;
    delete processWindow;
}

void FDOTool::parseCommandLine() {
    QCommandLineParser parser;
    parser.setApplicationDescription(tr("Tool per l'integrazione della firma digitale su Odoo"));
    parser.addHelpOption();
    parser.addVersionOption();

    QList<QCommandLineOption> commandLineOptions;
    commandLineOptions.append(
            QCommandLineOption(QStringList() << "l" << "list-certificates", tr("Elenco dei certificati disponibili")));
    commandLineOptions.append(
            QCommandLineOption(QStringList() << "c" << "list-devices", tr("Elenco dei dispositivi disponibili")));
    parser.addOptions(commandLineOptions);

    parser.addPositionalArgument("url", tr("URL fornito da Odoo"));

    parser.process(*this);

    const QStringList options = parser.optionNames();
    const QStringList args = parser.positionalArguments();

    if (!options.empty()) {
        if (parser.isSet("l")) {

        } else if (parser.isSet("c")) {

        } else {
            parser.showHelp(1);
        }
    }

    if (args.empty()) {
        // Error, show error dialog
        parser.showHelp(1);
        return;
    }

    mode = ODOO;

    for (const auto &arg : args) {
        QStringList items = arg.split("?");

        QUrlQuery urlQuery(items.at(1));
        if (urlQuery.isEmpty()) {
            continue;
        }

        QString rawData = urlQuery.queryItemValue("action");

        QByteArray byteArray;
        byteArray.append(rawData);
        QString data = QByteArray::fromBase64(byteArray);

        QStringList elems = data.trimmed().split("|");
        Action action(elems.at(0), elems.at(1));
        actions.append(action);
    }
}

int FDOTool::run() {
    qDebug() << "RUN";

    switch (mode) {

        case MAIN:
            mainWindow->show();
            QtConcurrent::run(this, &FDOTool::doMain);
            break;

        case ODOO:
            processWindow->show();

            connect(odooWorker, SIGNAL(rpcError(QString, QString)), processWindow, SLOT(rpcError(QString, QString)));

            connect(odooWorker, SIGNAL(updateProgress(int, int)), processWindow, SLOT(updateProgress(int, int)));
            connect(odooWorker, SIGNAL(updateStep(QString)), processWindow, SLOT(updateStep(QString)));

            connect(odooWorker, SIGNAL(updateAddress(QString)), processWindow, SLOT(updateAddress(QString)));
            connect(odooWorker, SIGNAL(updateUser(QString)), processWindow, SLOT(updateUser(QString)));
            connect(odooWorker, SIGNAL(updateJobs(int)), processWindow, SLOT(updateJobs(int)));

            QtConcurrent::run(this, &FDOTool::doOdoo);

            break;
    }

    return QApplication::exec();
}

void FDOTool::waitAndClose() {
    QtConcurrent::run(this, &FDOTool::doWaitAndClose, false);
}

void FDOTool::doWaitAndClose(bool wait) {
    if (wait)
        QThread::sleep(2);

    closeAllWindows();
    exit();
}

void FDOTool::doMain() {

}

void FDOTool::doOdoo() {
    bool result = true;
    for (const auto &action : actions) {
        result = odooWorker->doAction(action);

        if (!result) {
            connect(processWindow, SIGNAL(waitAndClose()), this, SLOT(waitAndClose()));
            break;
        }
    }

    if (result)
        doWaitAndClose(true);
}
