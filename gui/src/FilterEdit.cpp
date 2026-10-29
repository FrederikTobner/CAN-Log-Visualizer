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

constexpr auto kSyntaxHelp =
    "<p>Comma-separated hex IDs. Press Enter to apply, clear the field to show everything.</p>"
    "<table>"
    "<tr><td><b>123</b></td><td>&nbsp;standard frame 123</td></tr>"
    "<tr><td><b>700:700</b></td><td>&nbsp;IDs where <i>ID &amp; 700 == 700</i> (700..7FF)</td></tr>"
    "<tr><td><b>12345678</b></td><td>&nbsp;extended frame (8 digits)</td></tr>"
    "<tr><td><b>123, 456</b></td><td>&nbsp;any of the listed IDs</td></tr>"
    "<tr><td><b>!7FF</b></td><td>&nbsp;everything except 7FF (always wins)</td></tr>"
    "</table>";

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
