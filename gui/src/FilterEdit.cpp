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

#include "FilterEdit.hpp"

namespace {

constexpr auto kSyntaxHelp = "Comma-separated hex IDs: ID, ID:MASK, !ID to exclude.\n"
                             "3 digits (or fewer) are standard IDs, 8 digits are extended IDs.\n"
                             "Example: 123, 700:700, !7FF";

constexpr auto kInvalidStyle = "QLineEdit { border: 1px solid #d33; }";

} // namespace

FilterEdit::FilterEdit(QWidget * parent) : QLineEdit(parent) {
    setPlaceholderText("Filter: ID[:MASK], !ID");
    setToolTip(kSyntaxHelp);
    connect(this, &QLineEdit::editingFinished, this, &FilterEdit::applyText);
}

void FilterEdit::applyText() {
    auto const filter = parseCanFilter(text());
    if (filter) {
        setStyleSheet({});
        setToolTip(kSyntaxHelp);
        emit filterChanged(*filter);
        return;
    }

    setStyleSheet(kInvalidStyle);
    setToolTip(filter.error());
    emit filterRejected(filter.error());
}
