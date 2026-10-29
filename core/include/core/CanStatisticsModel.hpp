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

/// @file CanStatisticsModel.hpp
/// @brief Defines `CanStatisticsModel`, a table model with one row per CAN ID.

#pragma once

#include "core/CanFrame.hpp"
#include "core/CanStatistics.hpp"

#include <QAbstractTableModel>
#include <QList>

/// @brief Table model showing the statistics of every CAN ID seen, ordered by ID.
/// @details Owns the `CanStatistics`. Views are notified once per batch of frames, not once per frame.
class CanStatisticsModel : public QAbstractTableModel {
    Q_OBJECT

  public:
    /// @brief Columns of the table model.
    enum Column {
        Id,
        Type,
        Count,
        Rate,
        CycleLast,
        CycleMin,
        CycleMean,
        CycleMax,
        Data,
        ColumnCount
    };

    /// @brief Role that returns a value suited for sorting: numbers for numeric columns, text otherwise.
    static constexpr int kSortRole = Qt::UserRole;

    /// @brief Constructs an empty model.
    /// @param parent The parent QObject, if any.
    explicit CanStatisticsModel(QObject * parent = nullptr);

    /// @brief Returns the number of rows in the model.
    /// @param parent The parent index, if any.
    /// @return The number of rows.
    int rowCount(QModelIndex const & parent = {}) const override;
    /// @brief Returns the number of columns in the model.
    /// @param parent The parent index, if any.
    /// @return The number of columns.
    int columnCount(QModelIndex const & parent = {}) const override;

    /// @brief Returns the data for the given index and role.
    /// @param index The model index.
    /// @param role The role for which data is requested (`Qt::DisplayRole` or `kSortRole`).
    /// @return The data for the given index and role.
    QVariant data(QModelIndex const & index, int role = Qt::DisplayRole) const override;

    /// @brief Returns the header data for the given section, orientation, and role.
    /// @param section The section (row or column).
    /// @param orientation The orientation (horizontal or vertical).
    /// @param role The role for which header data is requested.
    /// @return The header data.
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /// @brief Accounts for a batch of frames.
    /// @param frames The frames in arrival order.
    void addFrames(QList<CanFrame> const & frames);

    /// @brief Moves the statistics windows forward so that rates decay when no frames arrive.
    /// @param timestamp The new end of the windows in seconds.
    void advanceTo(double timestamp);

    /// @brief Changes the bitrate used for the bus load.
    /// @param bitrate The bitrate in bit/s.
    void setBitrate(unsigned bitrate);

    /// @brief Forgets all frames and removes all rows.
    void reset();

    /// @brief Returns the underlying statistics.
    /// @return The statistics.
    CanStatistics const & statistics() const;

  private:
    void notifyChanged(Column first, Column last);

    CanStatistics m_statistics;
};
