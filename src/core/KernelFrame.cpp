#include "core/KernelFrame.h"

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
