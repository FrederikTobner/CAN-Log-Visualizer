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

#include "MainWindow.hpp"

#include "FilterEdit.hpp"
#include "HelpDialog.hpp"
#include "StatisticsPanel.hpp"

#include "core/CanFilterProxyModel.hpp"
#include "core/CanStatisticsModel.hpp"
#include "core/CanTraceModel.hpp"
#include "core/LogFormat.hpp"
#include "core/SocketCanSource.hpp"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPushButton>
#include <QStatusBar>
#include <QTableView>
#include <QToolBar>

namespace {

constexpr auto kAppName = "CanLogVisualizer";
constexpr auto kLogFileFilter = "CAN logs (*.log);;All files (*)";
constexpr qsizetype kLiveMaxRows = 100000;
constexpr qsizetype kFileMaxRows = 1000000;

/// Interface name written to saved logs when the interface field is empty.
constexpr auto kFallbackInterface = "can0";

QString defaultLogName() {
    return QDateTime::currentDateTime().toString("'candump-'yyyy-MM-dd_HHmmss'.log'");
}

} // namespace

MainWindow::MainWindow(QString const & interfaceName, QWidget * parent)
    : QMainWindow(parent), m_model(new CanTraceModel(kLiveMaxRows, this)),
      m_proxy(new CanFilterProxyModel(m_model, this)), m_statistics(new CanStatisticsModel(this)),
      m_statisticsPanel(new StatisticsPanel(m_statistics, this)), m_source(new SocketCanSource(interfaceName, this)),
      m_view(new QTableView(this)), m_interfaceEdit(new QLineEdit(interfaceName, this)),
      m_filterEdit(new FilterEdit(this)), m_connectButton(new QPushButton("Connect", this)),
      m_autoScroll(new QCheckBox("Auto-scroll", this)), m_countLabel(new QLabel(this)) {
    setWindowTitle(kAppName);
    setMinimumSize(800, 600);

    m_interfaceEdit->setMaximumWidth(160);
    m_filterEdit->setMaximumWidth(260);
    m_autoScroll->setChecked(true);
    auto * clearButton = new QPushButton("Clear", this);
    clearButton->setToolTip("Remove all frames and reset the statistics");
    m_autoScroll->setToolTip("Keep the newest frame in view");
    m_interfaceEdit->setToolTip("SocketCAN interface, e.g. vcan0 or can0");

    auto * toolBar = addToolBar("Main");
    toolBar->setMovable(false);
    toolBar->addWidget(new QLabel("Interface: ", this));
    toolBar->addWidget(m_interfaceEdit);
    toolBar->addWidget(m_connectButton);
    toolBar->addWidget(m_filterEdit);
    toolBar->addWidget(clearButton);
    toolBar->addWidget(m_autoScroll);

    auto * fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Open Log...", QKeySequence::Open, this, &MainWindow::openLog);
    fileMenu->addAction("&Save Log As...", QKeySequence::SaveAs, this, &MainWindow::saveLog);
    fileMenu->addSeparator();
    fileMenu->addAction("&Quit", QKeySequence::Quit, this, &QWidget::close);

    auto * helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&Quick Reference", QKeySequence::HelpContents, this, [this] { showHelpDialog(this); });
    helpMenu->addAction("About &Qt", qApp, &QApplication::aboutQt);

    m_view->setModel(m_proxy);
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

    auto * statisticsDock = new QDockWidget("Statistics", this);
    statisticsDock->setWidget(m_statisticsPanel);
    addDockWidget(Qt::BottomDockWidgetArea, statisticsDock);
    menuBar()->addMenu("&View")->addAction(statisticsDock->toggleViewAction());

    statusBar()->addPermanentWidget(m_countLabel);
    statusBar()->showMessage("Disconnected");
    updateFrameCount();

    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::toggleConnection);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearTrace);
    connect(m_filterEdit, &FilterEdit::filterChanged, this, &MainWindow::onFilterChanged);
    connect(m_filterEdit, &FilterEdit::filterRejected, this,
            [this](QString const & reason) { statusBar()->showMessage("Invalid filter: " + reason); });
    connect(m_source, &CanSource::runningChanged, this, &MainWindow::onRunningChanged);
    connectSource(m_source);
}

MainWindow::~MainWindow() {
    // The sources are children and only die after this destructor. Stop them while the
    // window is still complete so none of their signals reach a half-destroyed window.
    m_source->stop();
    discardFileSource();
}

void MainWindow::connectSource(CanSource const * source) {
    connect(source, &CanSource::framesReceived, this, &MainWindow::onFramesReceived);
    connect(source, &CanSource::errorOccurred, this,
            [this](QString const & message) { statusBar()->showMessage(message); });
}

void MainWindow::toggleConnection() {
    if (m_source->isRunning()) {
        m_source->stop();
        return;
    }

    m_source->setInterfaceName(m_interfaceEdit->text().trimmed());
    // Frames only arrive from the event loop, so the trace can be switched after a successful start.
    if (m_source->start() && m_fileSource != nullptr) {
        showLiveTrace();
    }
}

void MainWindow::openLog() {
    QString const path = QFileDialog::getOpenFileName(this, "Open CAN log", {}, kLogFileFilter);
    if (!path.isEmpty()) {
        openLogFile(path);
    }
}

void MainWindow::openLogFile(QString const & path) {
    auto * source = new LogFileSource(path, this);
    connectSource(source);
    connect(source, &LogFileSource::finished, this, &MainWindow::onLogLoaded);

    // As with the live source, frames are delivered from the event loop after start() returned.
    if (source->start()) {
        showFileTrace(source);
    } else {
        source->deleteLater();
    }
}

void MainWindow::saveLog() {
    QString const path = QFileDialog::getSaveFileName(this, "Save CAN log", defaultLogName(), kLogFileFilter);
    if (path.isEmpty()) {
        return;
    }

    QString const interfaceName = m_interfaceEdit->text().trimmed();
    auto const frames = m_proxy->visibleFrames();
    auto const result = saveLogFile(path, frames, interfaceName.isEmpty() ? kFallbackInterface : interfaceName);
    if (result) {
        statusBar()->showMessage(QString("Saved %1 frames to %2").arg(*result).arg(QFileInfo(path).fileName()));
    } else {
        statusBar()->showMessage("Cannot save log: " + result.error());
    }
}

void MainWindow::showLiveTrace() {
    discardFileSource();
    setWindowTitle(kAppName);
    m_statisticsPanel->setLive(true);
    resetTrace(kLiveMaxRows);
}

void MainWindow::showFileTrace(LogFileSource * source) {
    m_source->stop();
    discardFileSource();
    m_fileSource = source;
    m_statisticsPanel->setLive(false);
    setWindowTitle(QString("%1 - %2").arg(kAppName, QFileInfo(source->path()).fileName()));
    resetTrace(kFileMaxRows);
}

void MainWindow::discardFileSource() {
    if (m_fileSource == nullptr) {
        return;
    }

    m_fileSource->stop();
    m_fileSource->deleteLater();
    m_fileSource = nullptr;
}

void MainWindow::resetTrace(qsizetype maxRows) {
    clearTrace();
    m_model->setMaxRows(maxRows);
}

void MainWindow::clearTrace() {
    m_model->clear();
    m_statistics->reset();
    m_totalFrames = 0;
    updateFrameCount();
}

void MainWindow::onFilterChanged(CanFilter filter) {
    m_proxy->setFilter(std::move(filter));
    updateFrameCount();
}

void MainWindow::onLogLoaded(LogLoadSummary const & summary) {
    QString message = QString("Loaded %1 frames").arg(summary.frames);
    if (summary.frames > m_model->rowCount()) {
        message += QString(", only the last %1 kept").arg(m_model->rowCount());
    }
    if (summary.skippedLines > 0) {
        message += QString(", %1 lines skipped (%2)").arg(summary.skippedLines).arg(summary.firstProblem);
    }
    statusBar()->showMessage(message);
}

void MainWindow::onFramesReceived(QList<CanFrame> const & frames) {
    m_model->appendFrames(frames);
    m_statistics->addFrames(frames);
    m_totalFrames += static_cast<qulonglong>(frames.size());
    updateFrameCount();

    if (m_autoScroll->isChecked() && m_fileSource == nullptr) {
        m_view->scrollToBottom();
    }
}

void MainWindow::onRunningChanged(bool running) {
    m_connectButton->setText(running ? "Disconnect" : "Connect");
    m_interfaceEdit->setEnabled(!running);
    statusBar()->showMessage(running ? QString("Connected to %1").arg(m_source->interfaceName()) : "Disconnected");
}

void MainWindow::updateFrameCount() {
    m_countLabel->setText(QString("Frames: %1 (shown: %2)").arg(m_totalFrames).arg(m_proxy->rowCount()));
}
