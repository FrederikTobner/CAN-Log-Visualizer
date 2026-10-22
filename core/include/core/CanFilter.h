#pragma once

#include "core/CanFrame.h"

#include <QString>

#include <cstdint>
#include <expected>
#include <vector>

/// @brief Matches frames of one type (standard or extended) whose identifier equals
/// `id` on every bit set in `mask`.
struct IdRule {
    uint32_t id = 0;
    uint32_t mask = 0;
    bool extended = false;

    bool matches(CanFrame const & frame) const;
};

/// @brief Filter deciding which frames are shown.
/// @details A frame passes if it matches no exclude rule and,
/// when any include rules exist, at least one include rule is matched.
/// An empty filter passes everything.
struct CanFilter {
    std::vector<IdRule> include;
    std::vector<IdRule> exclude;

    bool matches(CanFrame const & frame) const;
};

/// @brief Parses comma-separated terms `ID`, `ID:MASK`, `!ID` and `!ID:MASK` in hex.
/// @details An ID of 8 digits is extended, 1-3 digits are standard; the mask defaults
/// to all bits of that type. Blank text gives an empty filter.
std::expected<CanFilter, QString> parseCanFilter(QString const & text);
