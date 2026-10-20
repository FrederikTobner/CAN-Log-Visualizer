#pragma once

#include "core/CanFrame.h"
#include "core/Result.h"

#include <QString>

#include <cstdint>
#include <vector>

/// Matches frames of one type (standard or extended) whose identifier equals
/// `id` on every bit set in `mask`.
struct IdRule {
    uint32_t id = 0;
    uint32_t mask = 0;
    bool extended = false;

    bool matches(CanFrame const & frame) const;
};

/// Decides which frames are shown. A frame passes if it matches no exclude
/// rule and, when include rules exist, at least one include rule. An empty
/// filter passes everything.
struct CanFilter {
    std::vector<IdRule> include;
    std::vector<IdRule> exclude;

    bool matches(CanFrame const & frame) const;
};

/// Parses comma-separated terms `ID`, `ID:MASK`, `!ID` and `!ID:MASK` in hex.
/// An ID of 8 digits is extended, 1-3 digits are standard; the mask defaults
/// to all bits of that type. Blank text gives an empty filter.
Result<CanFilter> parseCanFilter(QString const & text);
