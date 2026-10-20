#pragma once

#include "core/CanFrame.h"
#include "core/LogFileSource.h"

#include <QList>
#include <QMainWindow>

struct CanFilter;
class CanFilterProxyModel;
class CanSource;
class CanTraceModel;
class FilterEdit;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableView;
class SocketCanSource;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QString const & interfaceName, QWidget * parent = nullptr);
    ~MainWindow() override;

    /// Replaces the trace with the contents of a `candump` log file.
    void openLogFile(QString const & path);

  private:
    void toggleConnection();
    void clearTrace();
    void openLog();
    void saveLog();
    void onFramesReceived(QList<CanFrame> const & frames);
    void onRunningChanged(bool running);
    void onLogLoaded(LogLoadSummary const & summary);
    void onFilterChanged(CanFilter filter);

    void connectSource(CanSource const * source);
    void showLiveTrace();
    void showFileTrace(LogFileSource * source);
    void discardFileSource();
    void resetTrace(qsizetype maxRows);
    void updateFrameCount();

    CanTraceModel * m_model;
    CanFilterProxyModel * m_proxy;
    SocketCanSource * m_source;
    /// Set while the trace shows a log file (loading or loaded).
    LogFileSource * m_fileSource = nullptr;
    QTableView * m_view;
    QLineEdit * m_interfaceEdit;
    FilterEdit * m_filterEdit;
    QPushButton * m_connectButton;
    QCheckBox * m_autoScroll;
    QLabel * m_countLabel;
    qulonglong m_totalFrames = 0;
};
