#pragma once

#include "core/CanSource.h"

class QSocketNotifier;

/// Receives classic CAN frames from a Linux SocketCAN interface using a raw
/// non-blocking socket. Runs on the thread it lives in (event-loop driven).
class SocketCanSource final : public CanSource {
    Q_OBJECT

  public:
    explicit SocketCanSource(QString interfaceName = "vcan0", QObject * parent = nullptr);
    ~SocketCanSource() override;

    QString interfaceName() const;
    void setInterfaceName(QString name);

    bool start() override;
    void stop() override;
    bool isRunning() const override;

  private:
    void drainSocket();
    void fail(QString const & message);

    QString m_interfaceName;
    int m_fd = -1;
    QSocketNotifier * m_notifier = nullptr;
};
