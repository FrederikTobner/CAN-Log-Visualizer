#include "gui/MainWindow.h"

#include "core/CanTraceModel.h"
#include "core/SocketCanSource.h"

#include <QCheckBox>
#include <QFontDatabase>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QTableView>
#include <QToolBar>

MainWindow::MainWindow(QString const & interfaceName, QWidget * parent)
    : QMainWindow(parent), m_model(new CanTraceModel(100000, this)), m_source(new SocketCanSource(interfaceName, this)),
      m_view(new QTableView(this)), m_interfaceEdit(new QLineEdit(interfaceName, this)),
      m_connectButton(new QPushButton("Connect", this)), m_autoScroll(new QCheckBox("Auto-scroll", this)),
      m_countLabel(new QLabel(this)) {
    setWindowTitle("CanLogVisualizer");
    setMinimumSize(800, 600);

    m_interfaceEdit->setMaximumWidth(160);
    m_autoScroll->setChecked(true);
    auto * clearButton = new QPushButton("Clear", this);

    auto * toolBar = addToolBar("Main");
    toolBar->setMovable(false);
    toolBar->addWidget(new QLabel("Interface: ", this));
    toolBar->addWidget(m_interfaceEdit);
    toolBar->addWidget(m_connectButton);
    toolBar->addWidget(clearButton);
    toolBar->addWidget(m_autoScroll);

    m_view->setModel(m_model);
    m_view->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->verticalHeader()->setVisible(false);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->setColumnWidth(CanTraceModel::Number, 80);
    m_view->setColumnWidth(CanTraceModel::Time, 110);
    m_view->setColumnWidth(CanTraceModel::Id, 100);
    m_view->setColumnWidth(CanTraceModel::Type, 90);
    m_view->setColumnWidth(CanTraceModel::Dlc, 50);
    setCentralWidget(m_view);

    statusBar()->addPermanentWidget(m_countLabel);
    statusBar()->showMessage("Disconnected");
    updateFrameCount();

    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::toggleConnection);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearTrace);
    connect(m_source, &CanSource::framesReceived, this, &MainWindow::onFramesReceived);
    connect(m_source, &CanSource::runningChanged, this, &MainWindow::onRunningChanged);
    connect(m_source, &CanSource::errorOccurred, this,
            [this](QString const & message) { statusBar()->showMessage(message); });
}

void MainWindow::toggleConnection() {
    if (m_source->isRunning()) {
        m_source->stop();
        return;
    }

    m_source->setInterfaceName(m_interfaceEdit->text().trimmed());
    m_source->start();
}

void MainWindow::clearTrace() {
    m_model->clear();
    m_totalFrames = 0;
    updateFrameCount();
}

void MainWindow::onFramesReceived(QList<CanFrame> const & frames) {
    m_model->appendFrames(frames);
    m_totalFrames += static_cast<qulonglong>(frames.size());
    updateFrameCount();

    if (m_autoScroll->isChecked()) {
        m_view->scrollToBottom();
    }
}

void MainWindow::onRunningChanged(bool running) {
    m_connectButton->setText(running ? "Disconnect" : "Connect");
    m_interfaceEdit->setEnabled(!running);
    statusBar()->showMessage(running ? QString("Connected to %1").arg(m_source->interfaceName()) : "Disconnected");
}

void MainWindow::updateFrameCount() {
    m_countLabel->setText(QString("Frames: %1").arg(m_totalFrames));
}
