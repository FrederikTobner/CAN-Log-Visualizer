#include "FilterEdit.h"

namespace {

constexpr auto kSyntaxHelp = "Comma-separated hex IDs: ID, ID:MASK, !ID to exclude.\n"
                             "3 digits (or fewer) are standard IDs, 8 digits are extended IDs.\n"
                             "Example: 123, 700:700, !7FF";

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
