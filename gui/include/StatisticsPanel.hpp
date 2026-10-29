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

/// @file StatisticsPanel.hpp
/// @brief Declares the `StatisticsPanel` class, which shows bus and per-ID statistics.

#pragma once

#include <QWidget>

class CanStatisticsModel;
class QComboBox;
class QLabel;
class QTimer;

/// @brief Summary line, bitrate selection and per-ID table for a `CanStatisticsModel`.
/// @details The summary is refreshed on a timer instead of per frame. While live, the same timer moves the statistics
/// windows to the current time so that rates fall to zero when the bus goes quiet.
class StatisticsPanel final : public QWidget {
    Q_OBJECT

  public:
    /// @brief Constructs the panel.
    /// @param model The statistics to show. It must outlive the panel.
    /// @param parent The parent widget, if any.
    explicit StatisticsPanel(CanStatisticsModel * model, QWidget * parent = nullptr);

    /// @brief Selects whether rates decay with the wall clock (live traffic) or stay as they were (log file).
    /// @param live True for live traffic.
    void setLive(bool live);

  private:
    void refresh();
    void applyBitrate(QString const & text);

    CanStatisticsModel * m_model;
    QLabel * m_summary;
    QComboBox * m_bitrate;
    QTimer * m_timer;
    bool m_live = true;
};
