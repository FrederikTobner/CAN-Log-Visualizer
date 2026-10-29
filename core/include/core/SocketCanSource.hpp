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

/// @file SocketCanSource.hpp
/// @brief Defines the `SocketCanSource` class for receiving CAN frames from a Linux SocketCAN interface.
#pragma once

#include "core/CanSource.hpp"

class QSocketNotifier;

/// @brief Receives classic CAN frames from a Linux SocketCAN interface using a raw
/// non-blocking socket. Runs on the thread it lives in (event-loop driven).
class SocketCanSource final : public CanSource {
    Q_OBJECT

  public:
    /// @brief Constructs a `SocketCanSource` for the given interface.
    /// @param interfaceName The name of the CAN interface.
    /// @param parent The parent QObject, if any.
    explicit SocketCanSource(QString interfaceName = "vcan0", QObject * parent = nullptr);
    ~SocketCanSource() override;

    /// @brief Returns the name of the CAN interface.
    /// @return The interface name.
    QString interfaceName() const;

    /// @brief Sets the name of the CAN interface.
    /// @param name The new interface name.
    /// @details This can only be called when the source is not running.
    void setInterfaceName(QString name);

    /// @brief Starts receiving frames from the CAN interface.
    /// @return `true` if the source started successfully, `false` otherwise.
    bool start() override;

    /// @brief Stops receiving frames from the CAN interface.
    void stop() override;

    /// @brief Checks if the source is currently running.
    /// @return `true` if the source is running, `false` otherwise.
    bool isRunning() const override;

  private:
    void drainSocket();
    void releaseSocket();
    void fail(QString const & message);

    QString m_interfaceName;
    int m_fd = -1;
    QSocketNotifier * m_notifier = nullptr;
};
