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

/// @file CanFilter.hpp
/// @brief Defines structures and functions for filtering CAN frames.

#pragma once

#include "core/CanFrame.hpp"

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
