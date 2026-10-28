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

/// @file SlidingWindow.hpp
/// @brief Defines `SlidingWindow`, which sums weighted samples over the most recent stretch of time.

#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <limits>

/// @brief Counts and sums weighted samples that lie within the last `length` seconds.
/// @details Time is taken from the sample timestamps, never from a clock. The window ends at the newest timestamp seen
/// (or at the time passed to `advanceTo`) and covers the half-open range `(end - length, end]`. Timestamps are expected
/// to arrive roughly in order.
class SlidingWindow {
  public:
    /// @brief Constructs an empty window.
    /// @param lengthSeconds The length of the window in seconds.
    explicit SlidingWindow(double lengthSeconds);

    /// @brief Adds a sample. Samples that already lie before the window are ignored.
    /// @param timestamp The time of the sample in seconds.
    /// @param weight The weight of the sample.
    void add(double timestamp, uint32_t weight = 1);

    /// @brief Moves the end of the window forward, dropping samples that fall out. Earlier times are ignored.
    /// @param timestamp The new end of the window in seconds.
    void advanceTo(double timestamp);

    /// @brief Removes all samples and forgets the window position.
    void clear();

    /// @brief Returns the length of the window in seconds.
    /// @return The window length.
    double length() const;

    /// @brief Returns the number of samples inside the window.
    /// @return The sample count.
    size_t count() const;

    /// @brief Returns the summed weight of the samples inside the window.
    /// @return The total weight.
    uint64_t weight() const;

  private:
    struct Sample {
        double timestamp;
        uint32_t weight;
    };

    double m_length;
    double m_end = -std::numeric_limits<double>::infinity();
    std::deque<Sample> m_samples;
    uint64_t m_weight = 0;
};
