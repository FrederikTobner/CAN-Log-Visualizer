#pragma once

#include "core/CanFrame.h"

#include <QList>
#include <QObject>
#include <QString>

/// @brief Something that produces CAN frames (a live interface or a log file).
class CanSource : public QObject {
    Q_OBJECT

  public:
    using QObject::QObject;

    /// @brief Starts delivering frames. 
    /// @return Returns false on failure, in which case `errorOccurred` has been emitted with the reason. True otherwise.
    virtual bool start() = 0;

    /// @brief Stops delivering frames.
    virtual void stop() = 0;

    /// @brief Checks if the source is currently running.
    /// @return True if running, false otherwise.
    virtual bool isRunning() const = 0;

  signals:
    /// @brief Emitted when new frames are available.
    /// @param frames The frames received since the previous emission, in arrival order.
    void framesReceived(QList<CanFrame> frames);
    /// @brief Emitted when an error occurs.
    /// @param message The error message.
    void errorOccurred(QString message);
    /// @brief Emitted when the running state changes.
    /// @param running True if the source is now running, false otherwise.
    void runningChanged(bool running);
};
