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

#include "core/SlidingWindow.hpp"

SlidingWindow::SlidingWindow(double lengthSeconds) : m_length(lengthSeconds) {
}

void SlidingWindow::add(double timestamp, uint32_t weight) {
    advanceTo(timestamp);
    if (timestamp <= m_end - m_length) {
        return;
    }

    m_samples.push_back({.timestamp = timestamp, .weight = weight});
    m_weight += weight;
}

void SlidingWindow::advanceTo(double timestamp) {
    if (timestamp <= m_end) {
        return;
    }

    m_end = timestamp;
    while (!m_samples.empty() && m_samples.front().timestamp <= m_end - m_length) {
        m_weight -= m_samples.front().weight;
        m_samples.pop_front();
    }
}

void SlidingWindow::clear() {
    m_samples.clear();
    m_weight = 0;
    m_end = -std::numeric_limits<double>::infinity();
}

double SlidingWindow::length() const {
    return m_length;
}

size_t SlidingWindow::count() const {
    return m_samples.size();
}

uint64_t SlidingWindow::weight() const {
    return m_weight;
}
