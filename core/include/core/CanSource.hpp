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

/// @file CanSource.hpp
/// @brief Defines the `CanSource` class, which represents a source of CAN frames.
#pragma once

#include "core/CanFrame.hpp"

#include <QList>
#include <QObject>
#include <QString>

/// @brief Something that produces CAN frames (a live interface or a log file).
class CanSource : public QObject {
    Q_OBJECT

  public:
    using QObject::QObject;

    /// @brief Starts delivering frames.
    /// @return Returns false on failure, in which case `errorOccurred` has been emitted with the reason. True
    /// otherwise.
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
