#pragma once

#include "core/CanFilter.h"
#include "core/CanTraceModel.h"

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
