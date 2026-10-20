#pragma once

#include "core/CanFilter.h"
#include "core/CanTraceModel.h"

#include <QSortFilterProxyModel>

#include <vector>

/// Shows only the frames of a `CanTraceModel` that pass a `CanFilter`. The
/// source model keeps every frame, so removing the filter shows them again.
class CanFilterProxyModel final : public QSortFilterProxyModel {
    Q_OBJECT

  public:
    explicit CanFilterProxyModel(CanTraceModel * source, QObject * parent = nullptr);

    void setFilter(CanFilter filter);

    /// The frames currently shown, in row order.
    std::vector<CanFrame> visibleFrames() const;

  protected:
    bool filterAcceptsRow(int sourceRow, QModelIndex const & sourceParent) const override;

  private:
    CanTraceModel const * m_trace;
    CanFilter m_filter;
};
