/*****************************************************************************
 * Copyright (C) 2026 by Frederik Tobner                                     *
 *                                                                           *
 * This file is part of the CAN Log Visualizer.                              *
 *                                                                           *
 * Permission to use, copy, modify, and distribute this software and its     *
 * documentation under the terms of the GNU Affero General Public License is *
 * hereby granted.                                                           *
 * No representations are made about the suitability of this software for    *
 * any purpose.                                                              *
 * It is provided "as is" without express or implied warranty.               *
 * See the <https://www.gnu.org/licenses/gpl-3.0.html>                       *
 * GNU General Public License version 3 or later.                            *
 * License for more details.                                                 *
 ****************************************************************************/

#include <QApplication>
#include <QCommandLineParser>

#include "MainWindow.hpp"

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
