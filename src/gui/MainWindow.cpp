#include "gui/MainWindow.h"

#include <QLabel>
#include <QStatusBar>

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent) {
    setWindowTitle("CanLogVisualizer");
    setMinimumSize(800, 600);

    auto * placeholder = new QLabel("CanLogVisualizer", this);
    placeholder->setAlignment(Qt::AlignCenter);
    setCentralWidget(placeholder);

    statusBar()->showMessage("Ready");
}
