#pragma once

#include "core/CanFilter.h"

#include <QLineEdit>

/// Line edit for CAN ID filters. Applies the text when editing finishes
/// (Enter or focus loss) and marks invalid input instead of applying it.
class FilterEdit final : public QLineEdit {
    Q_OBJECT

  public:
    explicit FilterEdit(QWidget * parent = nullptr);

  signals:
    void filterChanged(CanFilter filter);
    void filterRejected(QString reason);

  private:
    void applyText();
};
