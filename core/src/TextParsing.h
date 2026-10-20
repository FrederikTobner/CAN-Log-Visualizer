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
