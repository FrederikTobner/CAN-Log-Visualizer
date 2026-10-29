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
#pragma once

#include "core/CanFrame.hpp"

#include <QString>

#include <expected>
#include <span>
#include <string_view>

/// The `candump -l` / `candump -L` log format, one frame per line:
/// `(1436509052.249713) vcan0 123#DEADBEEF`. Remote frames are written as
/// `123#R` or `123#R3` (with DLC). The interface name is not part of `CanFrame`.

/// @brief True for lines that carry no frame: blank lines and `#` comments.
bool isBlankOrComment(std::string_view line);

/// @brief Parses one log line.
/// @details CAN FD, error frames and malformed lines give an error
/// text describing the problem.
/// @param line The log line to parse.
/// @return The parsed `CanFrame` on success, or an error message on failure.
std::expected<CanFrame, QString> parseLogLine(std::string_view line);

/// @brief Formats a frame as a log line without the trailing newline.
/// @param frame The frame to format.
/// @param interfaceName The name of the CAN interface.
/// @return The formatted log line.
QString formatLogLine(CanFrame const & frame, QString const & interfaceName);

/// @brief Writes the frames to `path` atomically: a failure leaves no partial file.
/// @return The number of frames written, or an error message on failure.
std::expected<qsizetype, QString> saveLogFile(QString const & path, std::span<CanFrame const> frames,
                                              QString const & interfaceName);
