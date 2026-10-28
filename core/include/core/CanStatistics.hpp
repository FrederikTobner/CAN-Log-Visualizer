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

/// @file CanStatistics.hpp
/// @brief Defines `CanStatistics`, which aggregates frames into bus-wide and per-ID numbers.

#pragma once

#include "core/CanFrame.hpp"
#include "core/IdStatistics.hpp"
#include "core/SlidingWindow.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

/// @brief The statistics of one CAN ID.
struct IdEntry {
    IdKey key;
    IdStatistics statistics;
};

/// @brief Frame rate, bus load and per-ID statistics of a stream of frames.
/// @details All numbers derive from frame timestamps, so live traffic and log files are treated alike. Rates and the
/// bus load look at the last `windowSeconds` of frame time. The bus load is an upper bound because it assumes
/// worst-case bit stuffing (see `maxFrameBits`).
class CanStatistics {
  public:
    /// @brief Constructs empty statistics.
    /// @param windowSeconds The length of the window used for rates and load.
    /// @param bitrate The bus bitrate in bit/s used to compute the load.
    explicit CanStatistics(double windowSeconds = 1.0, unsigned bitrate = 500'000);

    /// @brief Accounts for a frame.
    /// @param frame The frame.
    void add(CanFrame const & frame);

    /// @brief Moves all windows forward so that rates decay when no frames arrive.
    /// @param timestamp The new end of the windows in seconds. Earlier times are ignored.
    void advanceTo(double timestamp);

    /// @brief Forgets all frames. The bitrate is kept.
    void reset();

    /// @brief Changes the bitrate used for the bus load.
    /// @param bitrate The bitrate in bit/s.
    void setBitrate(unsigned bitrate);

    /// @brief Returns the bitrate used for the bus load.
    /// @return The bitrate in bit/s.
    unsigned bitrate() const;

    /// @brief Returns the number of frames seen since the last reset.
    /// @return The frame count.
    uint64_t totalFrames() const;

    /// @brief Returns the frames per second within the window.
    /// @return The rate in Hz.
    double frameRate() const;

    /// @brief Returns the share of the bus capacity used within the window, as an upper bound.
    /// @return The load from 0.0 to 1.0 (it exceeds 1.0 if the traffic is faster than the bitrate allows).
    double busLoad() const;

    /// @brief Returns the newest timestamp seen.
    /// @return The timestamp in seconds, or nothing before the first frame.
    std::optional<double> latestTimestamp() const;

    /// @brief Returns the per-ID statistics.
    /// @return The entries sorted by `IdKey`.
    std::span<IdEntry const> ids() const;

    /// @brief Returns the position where an ID is, or would be inserted, in `ids()`.
    /// @param key The ID.
    /// @return The index of the first entry that is not ordered before the key.
    size_t lowerBound(IdKey const & key) const;

    /// @brief Checks if frames with the given ID have been seen.
    /// @param key The ID.
    /// @return True if there is an entry for the ID.
    bool contains(IdKey const & key) const;

  private:
    unsigned m_bitrate;
    SlidingWindow m_window;
    uint64_t m_totalFrames = 0;
    std::optional<double> m_latest;
    std::vector<IdEntry> m_ids;
};
