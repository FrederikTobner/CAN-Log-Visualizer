#pragma once

#include "core/CanFrame.h"

#include <linux/can.h>

/// @brief Converts a kernel `can_frame` into a `CanFrame`.
/// @details Splits the flag bits from the identifier and clamps the length to 8.
CanFrame frameFromKernel(can_frame const & raw, double timestamp);
