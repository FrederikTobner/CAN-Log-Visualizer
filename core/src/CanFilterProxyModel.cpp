#include "core/CanFilterProxyModel.h"

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
