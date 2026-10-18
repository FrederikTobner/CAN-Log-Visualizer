#pragma once

#include "core/CanFrame.h"

#include <QList>
#include <QObject>
#include <QString>

/// Something that produces CAN frames (a live interface, later a log file).
class CanSource : public QObject {
    Q_OBJECT

  public:
    using QObject::QObject;

    /// Starts delivering frames. Returns false on failure, in which case
    /// `errorOccurred` has been emitted with the reason.
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;

  signals:
    /// Frames received since the previous emission, in arrival order.
    void framesReceived(QList<CanFrame> frames);
    void errorOccurred(QString message);
    void runningChanged(bool running);
};
