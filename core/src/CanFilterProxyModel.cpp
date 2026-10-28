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

#include "core/CanFilterProxyModel.hpp"

CanFilterProxyModel::CanFilterProxyModel(CanTraceModel * source, QObject * parent)
    : QSortFilterProxyModel(parent), m_trace(source) {
    setSourceModel(source);
}

void CanFilterProxyModel::setFilter(CanFilter filter) {
// invalidateFilter() is deprecated since Qt 6.13
// The replacement exists since 6.9.
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    beginFilterChange();
    m_filter = std::move(filter);
    endFilterChange(Direction::Rows);
#else
    m_filter = std::move(filter);
    invalidateFilter();
#endif
}

std::vector<CanFrame> CanFilterProxyModel::visibleFrames() const {
    std::vector<CanFrame> frames;
    frames.reserve(static_cast<size_t>(rowCount()));
    for (int row = 0; row < rowCount(); ++row) {
        frames.push_back(m_trace->frameAt(mapToSource(index(row, 0)).row()));
    }
    return frames;
}

bool CanFilterProxyModel::filterAcceptsRow(int sourceRow, QModelIndex const & /*sourceParent*/) const {
    return m_filter.matches(m_trace->frameAt(sourceRow));
}
