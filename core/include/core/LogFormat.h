#pragma once

#include "core/CanFrame.h"
#include "core/Result.h"

#include <QString>

#include <span>
#include <string_view>

/// The `candump -l` / `candump -L` log format, one frame per line:
/// `(1436509052.249713) vcan0 123#DEADBEEF`. Remote frames are written as
/// `123#R` or `123#R3` (with DLC). The interface name is not part of `CanFrame`.

/// True for lines that carry no frame: blank lines and `#` comments.
bool isBlankOrComment(std::string_view line);

/// Parses one log line. CAN FD, error frames and malformed lines give an error
/// text describing the problem.
Result<CanFrame> parseLogLine(std::string_view line);

/// Formats a frame as a log line without the trailing newline.
QString formatLogLine(CanFrame const & frame, QString const & interfaceName);

/// Writes the frames to `path` atomically: a failure leaves no partial file.
/// Returns the number of frames written.
Result<qsizetype> saveLogFile(QString const & path, std::span<CanFrame const> frames, QString const & interfaceName);
