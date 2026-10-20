#include "core/LogFormat.h"

#include "TextParsing.h"

#include <QSaveFile>

#include <cmath>

namespace {

/// Removes and returns the next whitespace-separated token from `rest`.
std::string_view nextToken(std::string_view & rest) {
    rest = parsing::trim(rest);
    auto const end = rest.find_first_of(" \t");
    std::string_view const token = rest.substr(0, end);
    rest.remove_prefix(token.size());
    return token;
}

Result<double> parseTimestamp(std::string_view token) {
    if (!token.starts_with('(') || !token.ends_with(')')) {
        return Failure{QStringLiteral("timestamp must look like (seconds.microseconds)")};
    }

    token = token.substr(1, token.size() - 2);
    auto const seconds = parsing::parseDouble(token);
    if (!seconds || !std::isfinite(*seconds)) {
        return Failure{QStringLiteral("invalid timestamp '%1'").arg(parsing::toQString(token))};
    }
    return *seconds;
}

/// The part of a frame that follows the `#`: remote flag, DLC and data.
Result<CanFrame> parseRemotePayload(std::string_view dlcText) {
    CanFrame frame;
    frame.remote = true;
    if (dlcText.empty()) {
        return frame;
    }

    auto const dlc = parsing::parseHex(dlcText);
    if (!dlc || dlcText.size() != 1 || *dlc > frame.data.size()) {
        return Failure{QStringLiteral("invalid remote frame length '%1'").arg(parsing::toQString(dlcText))};
    }
    frame.dlc = static_cast<uint8_t>(*dlc);
    return frame;
}

Result<CanFrame> parseDataPayload(std::string_view hex) {
    CanFrame frame;
    if (hex.size() % 2 != 0 || hex.size() > 2 * frame.data.size()) {
        return Failure{QStringLiteral("data '%1' must be 0-8 bytes of hex digit pairs").arg(parsing::toQString(hex))};
    }

    frame.dlc = static_cast<uint8_t>(hex.size() / 2);
    for (size_t i = 0; i < frame.dlc; ++i) {
        auto const byte = parsing::parseHex(hex.substr(2 * i, 2));
        if (!byte) {
            return Failure{QStringLiteral("data '%1' is not hexadecimal").arg(parsing::toQString(hex))};
        }
        frame.data[i] = static_cast<uint8_t>(*byte);
    }
    return frame;
}

Result<CanFrame> parsePayload(std::string_view payload) {
    if (payload.starts_with('#')) {
        return Failure{QStringLiteral("CAN FD frames are not supported")};
    }
    if (payload.starts_with('R')) {
        return parseRemotePayload(payload.substr(1));
    }
    return parseDataPayload(payload);
}

QString formatPayload(CanFrame const & frame) {
    if (frame.remote) {
        return frame.dlc == 0 ? QStringLiteral("R") : QStringLiteral("R%1").arg(static_cast<int>(frame.dlc));
    }
    return formatData(frame).remove(' ');
}

} // namespace

bool isBlankOrComment(std::string_view line) {
    line = parsing::trim(line);
    return line.empty() || line.starts_with('#');
}

Result<CanFrame> parseLogLine(std::string_view line) {
    auto const timestampToken = nextToken(line);
    nextToken(line); // interface name
    auto const frameToken = nextToken(line);
    if (frameToken.empty() || !parsing::trim(line).empty()) {
        return Failure{QStringLiteral("expected '(timestamp) interface ID#DATA'")};
    }

    auto const hash = frameToken.find('#');
    if (hash == std::string_view::npos) {
        return Failure{QStringLiteral("frame '%1' has no '#' between ID and data").arg(parsing::toQString(frameToken))};
    }

    auto const timestamp = parseTimestamp(timestampToken);
    if (!timestamp) {
        return Failure{timestamp.error()};
    }
    auto const id = parsing::parseId(frameToken.substr(0, hash));
    if (!id) {
        return Failure{id.error()};
    }
    auto const payload = parsePayload(frameToken.substr(hash + 1));
    if (!payload) {
        return Failure{payload.error()};
    }

    CanFrame frame = *payload;
    frame.timestamp = *timestamp;
    frame.id = id->value;
    frame.extended = id->extended;
    return frame;
}

QString formatLogLine(CanFrame const & frame, QString const & interfaceName) {
    return QStringLiteral("(%1) %2 %3#%4")
        .arg(frame.timestamp, 0, 'f', 6)
        .arg(interfaceName, formatId(frame), formatPayload(frame));
}

Result<qsizetype> saveLogFile(QString const & path, std::span<CanFrame const> frames, QString const & interfaceName) {
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return Failure{file.errorString()};
    }

    for (CanFrame const & frame : frames) {
        file.write((formatLogLine(frame, interfaceName) + '\n').toUtf8());
    }

    // A failed write earlier makes the commit fail as well.
    if (!file.commit()) {
        return Failure{file.errorString()};
    }
    return static_cast<qsizetype>(frames.size());
}
