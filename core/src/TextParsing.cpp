#include "TextParsing.h"

#include "core/CanFrame.h"

#include <charconv>

namespace parsing {

namespace {

constexpr std::string_view kWhitespace = " \t\r\n";

/// Parses a number that must span the whole text.
template <typename Number, typename... Base> std::optional<Number> parseWhole(std::string_view text, Base... base) {
    Number value{};
    // NOLINTNEXTLINE(bugprone-suspicious-stringview-data-usage): the end is derived from the size
    char const * const begin = text.data();
    char const * const end = begin + text.size();
    auto const result = std::from_chars(begin, end, value, base...);
    if (result.ec != std::errc{} || result.ptr != end) {
        return std::nullopt;
    }
    return value;
}

} // namespace

std::string_view trim(std::string_view text) {
    auto const first = text.find_first_not_of(kWhitespace);
    if (first == std::string_view::npos) {
        return {};
    }
    return text.substr(first, text.find_last_not_of(kWhitespace) - first + 1);
}

std::optional<uint32_t> parseHex(std::string_view digits) {
    return parseWhole<uint32_t>(digits, 16);
}

std::optional<double> parseDouble(std::string_view text) {
    return parseWhole<double>(text);
}

std::expected<ParsedId, QString> parseId(std::string_view digits) {
    bool const extended = digits.size() == static_cast<size_t>(idDigits(true));
    if (digits.empty() || (!extended && digits.size() > static_cast<size_t>(idDigits(false)))) {
        return std::unexpected{
            QStringLiteral("ID '%1' must have 1-3 (standard) or 8 (extended) hex digits").arg(toQString(digits))};
    }

    auto const value = parseHex(digits);
    if (!value) {
        return std::unexpected{QStringLiteral("ID '%1' is not hexadecimal").arg(toQString(digits))};
    }
    if (*value > maxId(extended)) {
        return std::unexpected{QStringLiteral("ID '%1' is out of range").arg(toQString(digits))};
    }
    return ParsedId{.value = *value, .extended = extended};
}

QString toQString(std::string_view text) {
    return QString::fromUtf8(text.data(), static_cast<qsizetype>(text.size()));
}

} // namespace parsing
