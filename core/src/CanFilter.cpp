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

#include "core/CanFilter.hpp"

#include "TextParsing.hpp"

#include <algorithm>
#include <ranges>
#include <string>

namespace {

std::expected<IdRule, QString> parseRule(std::string_view term) {
    auto const colon = term.find(':');
    auto const id = parsing::parseId(parsing::trim(term.substr(0, colon)));
    if (!id) {
        return std::unexpected{id.error()};
    }

    IdRule rule{.id = id->value, .mask = maxId(id->extended), .extended = id->extended};
    if (colon != std::string_view::npos) {
        auto const maskText = parsing::trim(term.substr(colon + 1));
        auto const mask = parsing::parseHex(maskText);
        if (!mask || *mask > maxId(rule.extended)) {
            return std::unexpected{
                QStringLiteral("mask '%1' is not a hex value within the ID range").arg(parsing::toQString(maskText))};
        }
        rule.mask = *mask;
    }
    return rule;
}

} // namespace

bool IdRule::matches(CanFrame const & frame) const {
    return frame.extended == extended && (frame.id & mask) == (id & mask);
}

bool CanFilter::matches(CanFrame const & frame) const {
    auto const hits = [&frame](IdRule const & rule) { return rule.matches(frame); };
    return !std::ranges::any_of(exclude, hits) && (include.empty() || std::ranges::any_of(include, hits));
}

std::expected<CanFilter, QString> parseCanFilter(QString const & text) {
    std::string const utf8 = text.toStdString();
    CanFilter filter;

    for (auto const part : std::string_view{utf8} | std::views::split(',')) {
        std::string_view const term = parsing::trim({part.begin(), part.end()});
        if (term.empty()) {
            continue;
        }

        bool const isExclude = term.starts_with('!');
        auto const rule = parseRule(isExclude ? term.substr(1) : term);
        if (!rule) {
            return std::unexpected{QStringLiteral("'%1': %2").arg(parsing::toQString(term), rule.error())};
        }
        (isExclude ? filter.exclude : filter.include).push_back(*rule);
    }
    return filter;
}
