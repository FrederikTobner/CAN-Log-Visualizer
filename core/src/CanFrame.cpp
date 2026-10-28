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

#include "core/CanFrame.hpp"

#include <algorithm>

QString formatId(CanFrame const & frame) {
    int const width = idDigits(frame.extended);
    return QString::number(frame.id, 16).toUpper().rightJustified(width, '0');
}

QString formatData(CanFrame const & frame) {
    if (frame.remote) {
        return {};
    }

    QString text;
    auto const count = std::min<size_t>(frame.dlc, frame.data.size());
    for (size_t i = 0; i < count; ++i) {
        if (i > 0) {
            text += ' ';
        }
        text += QString("%1").arg(frame.data[i], 2, 16, QChar('0')).toUpper();
    }
    return text;
}
