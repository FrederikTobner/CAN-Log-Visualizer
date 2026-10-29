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

/// @file IdStatistics.hpp
/// @brief Defines `IdKey` and `IdStatistics`, the numbers kept for one CAN identifier.

#pragma once

#include "core/CanFrame.hpp"
#include "core/SlidingWindow.hpp"

#include <compare>
#include <cstdint>
#include <optional>

/// @brief Identifies a CAN ID. Standard and extended identifiers with the same value are different IDs.
struct IdKey {
    uint32_t id = 0;
    bool extended = false;

    /// @brief Orders by identifier value first, then standard before extended.
    auto operator<=>(IdKey const &) const = default;
};

/// @brief Returns the key of the ID a frame belongs to.
/// @param frame The frame.
/// @return The key of the frame's ID.
constexpr IdKey keyOf(CanFrame const & frame) {
    return {.id = frame.id, .extended = frame.extended};
}

/// @brief Time between consecutive frames of one ID, in seconds.
struct CycleTime {
    double last;
    double min;
    double max;
    double mean;
};

/// @brief Count, rate and cycle time of the frames with one CAN ID.
class IdStatistics {
  public:
    /// @brief Constructs empty statistics.
    /// @param windowSeconds The length of the window used for the rate.
    explicit IdStatistics(double windowSeconds);

    /// @brief Accounts for a frame of this ID.
    /// @param frame The frame.
    void add(CanFrame const & frame);

    /// @brief Moves the rate window forward so that the rate decays when no frames arrive.
    /// @param timestamp The new end of the window in seconds.
    void advanceTo(double timestamp);

    /// @brief Returns the number of frames seen.
    /// @return The frame count.
    uint64_t count() const;

    /// @brief Returns the frames per second within the window.
    /// @return The rate in Hz.
    double rate() const;

    /// @brief Returns the time between consecutive frames.
    /// @return The cycle time, or nothing before the second frame.
    std::optional<CycleTime> cycleTime() const;

    /// @brief Returns the most recent frame.
    /// @return The last frame, which is default-constructed before the first one.
    CanFrame const & lastFrame() const;

  private:
    void recordCycle(double seconds);

    SlidingWindow m_window;
    uint64_t m_count = 0;
    CanFrame m_lastFrame;
    uint64_t m_cycles = 0;
    double m_cycleSum = 0.0;
    CycleTime m_cycle{};
};
