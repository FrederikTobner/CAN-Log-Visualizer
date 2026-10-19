#include "core/CanFrame.h"

#include <algorithm>

QString formatId(CanFrame const & frame) {
    int const width = frame.extended ? 8 : 3;
    return QString::number(frame.id, 16).toUpper().rightJustified(width, '0');
}

QString formatData(CanFrame const & frame) {
    if (frame.remote) {
        return {};
    }

    QString text;
    auto const count = std::min<size_t>(frame.dlc, frame.data.size());
    for (size_t i = 0; i < count; ++i) {
        if (i > 0) {
            text += ' ';
        }
        text += QString("%1").arg(frame.data[i], 2, 16, QChar('0')).toUpper();
    }
    return text;
}
