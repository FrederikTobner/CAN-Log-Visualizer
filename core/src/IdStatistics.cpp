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

#include "core/IdStatistics.hpp"

#include <algorithm>

IdStatistics::IdStatistics(double windowSeconds) : m_window(windowSeconds) {
}

void IdStatistics::add(CanFrame const & frame) {
    // An out-of-order frame would give a negative cycle time, so it is not measured.
    if (m_count > 0 && frame.timestamp >= m_lastFrame.timestamp) {
        recordCycle(frame.timestamp - m_lastFrame.timestamp);
    }

    m_window.add(frame.timestamp);
    m_lastFrame = frame;
    ++m_count;
}

void IdStatistics::advanceTo(double timestamp) {
    m_window.advanceTo(timestamp);
}

uint64_t IdStatistics::count() const {
    return m_count;
}

double IdStatistics::rate() const {
    return static_cast<double>(m_window.count()) / m_window.length();
}

std::optional<CycleTime> IdStatistics::cycleTime() const {
    if (m_cycles == 0) {
        return std::nullopt;
    }

    CycleTime result = m_cycle;
    result.mean = m_cycleSum / static_cast<double>(m_cycles);
    return result;
}

CanFrame const & IdStatistics::lastFrame() const {
    return m_lastFrame;
}

void IdStatistics::recordCycle(double seconds) {
    if (m_cycles == 0) {
        m_cycle = {.last = seconds, .min = seconds, .max = seconds, .mean = 0.0};
    } else {
        m_cycle.last = seconds;
        m_cycle.min = std::min(m_cycle.min, seconds);
        m_cycle.max = std::max(m_cycle.max, seconds);
    }
    m_cycleSum += seconds;
    ++m_cycles;
}
