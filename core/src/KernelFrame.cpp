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

#include "core/KernelFrame.hpp"

#include <algorithm>
#include <cstring>

CanFrame frameFromKernel(can_frame const & raw, double timestamp) {
    CanFrame frame;
    frame.extended = (raw.can_id & CAN_EFF_FLAG) != 0;
    frame.remote = (raw.can_id & CAN_RTR_FLAG) != 0;
    frame.id = raw.can_id & (frame.extended ? CAN_EFF_MASK : CAN_SFF_MASK);
    frame.dlc = std::min<uint8_t>(raw.len, CAN_MAX_DLEN);
    if (!frame.remote) {
        std::memcpy(frame.data.data(), raw.data, frame.dlc);
    }
    frame.timestamp = timestamp;
    return frame;
}
