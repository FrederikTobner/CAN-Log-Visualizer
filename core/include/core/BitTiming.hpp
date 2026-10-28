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

/// @file BitTiming.hpp
/// @brief Defines the worst-case length of classic CAN frames on the wire.

#pragma once

#include "core/CanFrame.hpp"

#include <algorithm>
#include <cstdint>

/// @brief Largest number of bits a classic CAN frame occupies on the bus.
/// @details Counts SOF through EOF plus the 3-bit interframe space. Bit stuffing inserts one bit after five equal
/// bits; the worst case is `floor((s - 1) / 4)` extra bits for `s` stuff-susceptible bits (SOF up to and including the
/// CRC). The real length depends on the data, so this is an upper bound. Remote frames carry no data field.
/// @param frame The frame to measure.
/// @return The number of bits including stuffing and interframe space.
constexpr int maxFrameBits(CanFrame const & frame) {
    constexpr int kStandardOverhead = 47;
    constexpr int kExtendedOverhead = 67;
    // Bits from SOF to the end of the CRC sequence, where stuffing applies.
    constexpr int kStandardStuffed = 34;
    constexpr int kExtendedStuffed = 54;

    int const dataBits = frame.remote ? 0 : 8 * std::min<int>(frame.dlc, static_cast<int>(frame.data.size()));
    int const stuffedBits = (frame.extended ? kExtendedStuffed : kStandardStuffed) + dataBits;
    int const unstuffedBits = (frame.extended ? kExtendedOverhead : kStandardOverhead) + dataBits;
    return unstuffedBits + ((stuffedBits - 1) / 4);
}
