#pragma once

#include "core/CanFrame.h"

#include <QList>

inline CanFrame makeFrame(uint32_t id, bool extended = false, double timestamp = 0.0) {
    CanFrame frame;
    frame.id = id;
    frame.extended = extended;
    frame.timestamp = timestamp;
    return frame;
}

inline QList<CanFrame> makeFrames(std::initializer_list<uint32_t> ids) {
    QList<CanFrame> frames;
    for (uint32_t const id : ids) {
        frames.append(makeFrame(id));
    }
    return frames;
}
