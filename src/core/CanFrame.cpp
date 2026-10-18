#include "core/CanFrame.h"

QString formatId(CanFrame const & frame) {
    int const width = frame.extended ? 8 : 3;
    return QString::number(frame.id, 16).toUpper().rightJustified(width, '0');
}

QString formatData(CanFrame const & frame) {
    if (frame.remote) {
        return {};
    }

    QString text;
    for (int i = 0; i < frame.dlc && i < static_cast<int>(frame.data.size()); ++i) {
        if (i > 0) {
            text += ' ';
        }
        text += QString("%1").arg(frame.data[i], 2, 16, QChar('0')).toUpper();
    }
    return text;
}
