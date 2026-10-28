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

/// @file CanFilterProxyModel.hpp
/// @brief Defines a proxy model that filters CAN frames based on a `CanFilter`.
#pragma once

#include "core/CanFilter.hpp"
#include "core/CanTraceModel.hpp"

#include <QSortFilterProxyModel>

#include <vector>

/// @brief Shows only the frames of a `CanTraceModel` that pass a `CanFilter`.
/// @details The source model keeps every frame, so removing the filter shows them again.
class CanFilterProxyModel final : public QSortFilterProxyModel {
    Q_OBJECT

  public:
    /// @brief Constructs the proxy model.
    /// @param source The source `CanTraceModel`.
    /// @param parent The parent QObject, if any.
    explicit CanFilterProxyModel(CanTraceModel * source, QObject * parent = nullptr);

    /// @brief Sets the filter used to determine which frames are shown.
    /// @param filter The filter to apply.
    void setFilter(CanFilter filter);

    /// @brief The frames currently shown, in row order.
    std::vector<CanFrame> visibleFrames() const;

  protected:
    bool filterAcceptsRow(int sourceRow, QModelIndex const & sourceParent) const override;

  private:
    CanTraceModel const * m_trace;
    CanFilter m_filter;
};
