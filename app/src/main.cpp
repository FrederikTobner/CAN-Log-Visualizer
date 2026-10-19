#include <QApplication>
#include <QCommandLineParser>

#include "gui/MainWindow.h"

int main(int argc, char * argv[]) {
    QApplication const app(argc, argv);
    QApplication::setApplicationName("CanLogVisualizer");

    QCommandLineParser parser;
    parser.setApplicationDescription("CAN log visualizer");
    parser.addHelpOption();
    QCommandLineOption const interfaceOption({"i", "interface"}, "CAN interface to connect to.", "name", "vcan0");
    parser.addOption(interfaceOption);
    parser.process(app);

    MainWindow window(parser.value(interfaceOption));
    window.show();

    return QApplication::exec();
}
