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

/// @file KernelFrame.hpp
/// @brief Defines functions for converting kernel `can_frame` structures to `CanFrame`.
#pragma once

#include "core/CanFrame.hpp"

#include <linux/can.h>

/// @brief Converts a kernel `can_frame` into a `CanFrame`.
/// @details Splits the flag bits from the identifier and clamps the length to 8.
CanFrame frameFromKernel(can_frame const & raw, double timestamp);
