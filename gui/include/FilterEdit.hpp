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

/// @file FilterEdit.hpp
/// @brief Declares the `FilterEdit` class for editing CAN ID filters in the GUI.

#pragma once

#include "core/CanFilter.hpp"

#include <QLineEdit>

/// @brief Line edit for CAN ID filters.
/// @details Applies the text when editing finishes (onEnter or focus loss) and marks invalid input instead of applying
/// it.
class FilterEdit final : public QLineEdit {
    Q_OBJECT

  public:
    /// @brief Constructs a FilterEdit widget.
    /// @param parent The parent widget, if any.
    explicit FilterEdit(QWidget * parent = nullptr);

  signals:
    /// @brief Emitted when the filter is successfully changed.
    /// @param filter The new filter.
    void filterChanged(CanFilter filter);
    /// @brief Emitted when the filter change is rejected.
    /// @param reason The reason for rejection.
    void filterRejected(QString reason);

  private:
    void applyText();
};
