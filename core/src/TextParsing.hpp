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

/// @file TextParsing.hpp
/// @brief Declares functions for parsing and trimming text related to CAN IDs and numbers.

#pragma once

#include <QString>

#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>

/// Text helpers shared by the filter and log parsers. Internal to `core`.
namespace parsing {

/// A parsed identifier; the digit count selects the type (8 digits: extended).
struct ParsedId {
    uint32_t value;
    bool extended;
};

/// Removes leading and trailing whitespace.
std::string_view trim(std::string_view text);

/// Parses a whole string of hex digits; no sign, no `0x` prefix.
std::optional<uint32_t> parseHex(std::string_view digits);

/// Parses a decimal floating point number; the whole text must be consumed.
std::optional<double> parseDouble(std::string_view text);

/// Parses 1-3 (standard) or exactly 8 (extended) hex digits within the type's range.
std::expected<ParsedId, QString> parseId(std::string_view digits);

QString toQString(std::string_view text);

} // namespace parsing
