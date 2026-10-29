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

#include "core/CanStatisticsModel.hpp"

namespace {

constexpr double kMillisPerSecond = 1000.0;

QString milliseconds(double seconds) {
    return QString::number(seconds * kMillisPerSecond, 'f', 3);
}

/// Sorts by value first and standard before extended, like `IdKey`.
qulonglong sortKey(IdKey const & key) {
    return (static_cast<qulonglong>(key.id) * 2) + (key.extended ? 1 : 0);
}

/// Picks one value out of the cycle time, or nothing before the second frame.
QVariant cycleValue(IdStatistics const & statistics, double CycleTime::* member, bool forSorting) {
    auto const cycle = statistics.cycleTime();
    if (!cycle) {
        return forSorting ? QVariant() : QVariant(QStringLiteral("-"));
    }

    double const seconds = (*cycle).*member;
    return forSorting ? QVariant(seconds) : QVariant(milliseconds(seconds));
}

} // namespace

CanStatisticsModel::CanStatisticsModel(QObject * parent) : QAbstractTableModel(parent) {
}

int CanStatisticsModel::rowCount(QModelIndex const & parent) const {
    return parent.isValid() ? 0 : static_cast<int>(m_statistics.ids().size());
}

int CanStatisticsModel::columnCount(QModelIndex const & parent) const {
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant CanStatisticsModel::data(QModelIndex const & index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != kSortRole) || index.row() >= rowCount()) {
        return {};
    }

    bool const forSorting = role == kSortRole;
    IdEntry const & entry = m_statistics.ids()[static_cast<size_t>(index.row())];
    IdStatistics const & statistics = entry.statistics;
    switch (index.column()) {
    case Id:
        return forSorting ? QVariant(sortKey(entry.key)) : QVariant(formatId(statistics.lastFrame()));
    case Type:
        return entry.key.extended ? "EXT" : "STD";
    case Count:
        return static_cast<qulonglong>(statistics.count());
    case Rate:
        return forSorting ? QVariant(statistics.rate()) : QVariant(QString::number(statistics.rate(), 'f', 1));
    case CycleLast:
        return cycleValue(statistics, &CycleTime::last, forSorting);
    case CycleMin:
        return cycleValue(statistics, &CycleTime::min, forSorting);
    case CycleMean:
        return cycleValue(statistics, &CycleTime::mean, forSorting);
    case CycleMax:
        return cycleValue(statistics, &CycleTime::max, forSorting);
    case Data:
        return formatData(statistics.lastFrame());
    default:
        return {};
    }
}

QVariant CanStatisticsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case Id:
        return "ID";
    case Type:
        return "Type";
    case Count:
        return "Count";
    case Rate:
        return "Rate [Hz]";
    case CycleLast:
        return "Cycle last [ms]";
    case CycleMin:
        return "Cycle min [ms]";
    case CycleMean:
        return "Cycle mean [ms]";
    case CycleMax:
        return "Cycle max [ms]";
    case Data:
        return "Last data";
    default:
        return {};
    }
}

void CanStatisticsModel::addFrames(QList<CanFrame> const & frames) {
    if (frames.isEmpty()) {
        return;
    }

    for (CanFrame const & frame : frames) {
        IdKey const key = keyOf(frame);
        if (m_statistics.contains(key)) {
            m_statistics.add(frame);
            continue;
        }

        auto const row = static_cast<int>(m_statistics.lowerBound(key));
        beginInsertRows({}, row, row);
        m_statistics.add(frame);
        endInsertRows();
    }
    notifyChanged(Id, Data);
}

void CanStatisticsModel::advanceTo(double timestamp) {
    m_statistics.advanceTo(timestamp);
    notifyChanged(Rate, Rate);
}

void CanStatisticsModel::setBitrate(unsigned bitrate) {
    m_statistics.setBitrate(bitrate);
}

void CanStatisticsModel::reset() {
    beginResetModel();
    m_statistics.reset();
    endResetModel();
}

CanStatistics const & CanStatisticsModel::statistics() const {
    return m_statistics;
}

void CanStatisticsModel::notifyChanged(Column first, Column last) {
    int const rows = rowCount();
    if (rows > 0) {
        emit dataChanged(index(0, first), index(rows - 1, last));
    }
}
