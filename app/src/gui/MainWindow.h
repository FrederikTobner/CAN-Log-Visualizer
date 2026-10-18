#pragma once

#include "core/CanFrame.h"

#include <QList>
#include <QMainWindow>

class CanTraceModel;
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

  private:
    void toggleConnection();
    void clearTrace();
    void onFramesReceived(QList<CanFrame> const & frames);
    void onRunningChanged(bool running);
    void updateFrameCount();

    CanTraceModel * m_model;
    SocketCanSource * m_source;
    QTableView * m_view;
    QLineEdit * m_interfaceEdit;
    QPushButton * m_connectButton;
    QCheckBox * m_autoScroll;
    QLabel * m_countLabel;
    qulonglong m_totalFrames = 0;
};
