#include "core/CanTraceModel.h"

#include <algorithm>

CanTraceModel::CanTraceModel(qsizetype maxRows, QObject * parent)
    : QAbstractTableModel(parent), m_maxRows(std::max<qsizetype>(maxRows, 1)) {
}

int CanTraceModel::rowCount(QModelIndex const & parent) const {
    return parent.isValid() ? 0 : static_cast<int>(m_frames.size());
}

int CanTraceModel::columnCount(QModelIndex const & parent) const {
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant CanTraceModel::data(QModelIndex const & index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole || index.row() >= static_cast<int>(m_frames.size())) {
        return {};
    }

    CanFrame const & frame = m_frames[static_cast<size_t>(index.row())];
    switch (index.column()) {
    case Number:
        return m_firstNumber + static_cast<qulonglong>(index.row());
    case Time:
        return QString::number(frame.timestamp - m_timeOrigin.value_or(frame.timestamp), 'f', 6);
    case Id:
        return formatId(frame);
    case Type:
        return QString(frame.extended ? "EXT" : "STD") + (frame.remote ? " RTR" : "");
    case Dlc:
        return frame.dlc;
    case Data:
        return formatData(frame);
    default:
        return {};
    }
}

QVariant CanTraceModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case Number:
        return "#";
    case Time:
        return "Time [s]";
    case Id:
        return "ID";
    case Type:
        return "Type";
    case Dlc:
        return "DLC";
    case Data:
        return "Data";
    default:
        return {};
    }
}

void CanTraceModel::appendFrames(QList<CanFrame> const & frames) {
    if (frames.isEmpty()) {
        return;
    }

    if (!m_timeOrigin) {
        m_timeOrigin = frames.first().timestamp;
    }

    // Frames of this batch that cannot fit even into an empty model.
    qsizetype const skipped = std::max<qsizetype>(frames.size() - m_maxRows, 0);
    qsizetype const incoming = frames.size() - skipped;
    qsizetype const current = static_cast<qsizetype>(m_frames.size());
    qsizetype const overflow = std::max<qsizetype>(current + incoming - m_maxRows, 0);

    if (overflow > 0) {
        beginRemoveRows({}, 0, static_cast<int>(overflow) - 1);
        m_frames.erase(m_frames.begin(), m_frames.begin() + overflow);
        m_firstNumber += static_cast<qulonglong>(overflow);
        endRemoveRows();
    }
    m_firstNumber += static_cast<qulonglong>(skipped);

    int const first = static_cast<int>(m_frames.size());
    beginInsertRows({}, first, first + static_cast<int>(incoming) - 1);
    m_frames.insert(m_frames.end(), frames.begin() + skipped, frames.end());
    endInsertRows();
}

void CanTraceModel::clear() {
    beginResetModel();
    m_frames.clear();
    m_firstNumber = 1;
    m_timeOrigin.reset();
    endResetModel();
}
