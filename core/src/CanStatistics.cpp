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

#include "core/CanStatistics.hpp"

#include "core/BitTiming.hpp"

#include <algorithm>

CanStatistics::CanStatistics(double windowSeconds, unsigned bitrate) : m_bitrate(bitrate), m_window(windowSeconds) {
}

void CanStatistics::add(CanFrame const & frame) {
    ++m_totalFrames;
    m_latest = std::max(m_latest.value_or(frame.timestamp), frame.timestamp);
    m_window.add(frame.timestamp, static_cast<uint32_t>(maxFrameBits(frame)));

    IdKey const key = keyOf(frame);
    size_t const index = lowerBound(key);
    if (!contains(key)) {
        m_ids.insert(m_ids.begin() + static_cast<std::ptrdiff_t>(index),
                     IdEntry{.key = key, .statistics = IdStatistics(m_window.length())});
    }
    m_ids[index].statistics.add(frame);
}

void CanStatistics::advanceTo(double timestamp) {
    m_window.advanceTo(timestamp);
    for (IdEntry & entry : m_ids) {
        entry.statistics.advanceTo(timestamp);
    }
}

void CanStatistics::reset() {
    m_window.clear();
    m_totalFrames = 0;
    m_latest.reset();
    m_ids.clear();
}

void CanStatistics::setBitrate(unsigned bitrate) {
    m_bitrate = bitrate;
}

unsigned CanStatistics::bitrate() const {
    return m_bitrate;
}

uint64_t CanStatistics::totalFrames() const {
    return m_totalFrames;
}

double CanStatistics::frameRate() const {
    return static_cast<double>(m_window.count()) / m_window.length();
}

double CanStatistics::busLoad() const {
    if (m_bitrate == 0) {
        return 0.0;
    }
    return static_cast<double>(m_window.weight()) / (m_window.length() * static_cast<double>(m_bitrate));
}

std::optional<double> CanStatistics::latestTimestamp() const {
    return m_latest;
}

std::span<IdEntry const> CanStatistics::ids() const {
    return m_ids;
}

size_t CanStatistics::lowerBound(IdKey const & key) const {
    auto const it = std::ranges::lower_bound(m_ids, key, {}, &IdEntry::key);
    return static_cast<size_t>(it - m_ids.begin());
}

bool CanStatistics::contains(IdKey const & key) const {
    size_t const index = lowerBound(key);
    return index < m_ids.size() && m_ids[index].key == key;
}
