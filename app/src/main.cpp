#include <QApplication>
#include <QCommandLineParser>

#include "MainWindow.h"

int main(int argc, char * argv[]) {
    QApplication const app(argc, argv);
    QApplication::setApplicationName("CanLogVisualizer");

    QCommandLineParser parser;
    parser.setApplicationDescription("CAN log visualizer");
    parser.addHelpOption();
    QCommandLineOption const interfaceOption({"i", "interface"}, "CAN interface to connect to.", "name", "vcan0");
    parser.addPositionalArgument("file", "CAN log file (candump -l format) to open.", "[file]");
    parser.addOption(interfaceOption);
    parser.process(app);
    MainWindow window(parser.value(interfaceOption));
    window.show();

    if (auto const files = parser.positionalArguments(); !files.isEmpty()) {
        window.openLogFile(files.first());
    }

    return QApplication::exec();
}
