#pragma once

#include "core/CanFilter.h"

#include <QLineEdit>

/// @brief Line edit for CAN ID filters. 
/// @details Applies the text when editing finishes (onEnter or focus loss) and marks invalid input instead of applying it.
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
