#include <QtCore/QDebug>
#include <QtCore/QCommandLineParser>
#include <QtCore/QUrlQuery>
#include <QtCore/QFile>
#include <QtCore/QDate>
#include <QtCore/QDir>

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
    actions.clear();
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
    QFile file(QDir::home().filePath(QString("%1-commands.log").arg(APPLICATION_NAME_SHORT)));
    if (!file.open(QIODevice::Append)) {
        qDebug() << "Unable to open file";
        QApplication::exit(1);
    }

    QTextStream stream(&file);
    for (const auto &action : actions) {
        stream << QDateTime::currentDateTime().toString("yyyyMMdd HH:mm:ss") << " ---> "
               << action.getOdooUrl() << " | " << action.getToken() << endl;
    }

    file.close();

//    return QApplication::exec();
    return 0;
}
