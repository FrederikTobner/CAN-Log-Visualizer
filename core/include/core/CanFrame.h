#pragma once

#include <QString>

#include <array>
#include <cstdint>

/// @brief A classic CAN frame (up to 8 data bytes) as seen on the bus.
struct CanFrame {
    /// Identifier without flag bits: 11 bits (standard) or 29 bits (extended).
    uint32_t id = 0;
    bool extended = false;
    /// Remote transmission request; carries no data.
    bool remote = false;
    uint8_t dlc = 0;
    /// Only the first `dlc` bytes are valid (none for remote frames).
    std::array<uint8_t, 8> data{};
    /// Receive time in seconds since the Unix epoch.
    double timestamp = 0.0;
};

/// @brief Number of hex digits used to write an identifier of the given type.
constexpr int idDigits(bool extended) {
    return extended ? 8 : 3;
}

/// @brief Largest identifier of the given type (11 or 29 bits).
constexpr uint32_t maxId(bool extended) {
    return extended ? 0x1FFFFFFF : 0x7FF;
}

/// @brief Uppercase hex identifier, 3 digits (standard) or 8 digits (extended).
QString formatId(CanFrame const & frame);

/// @brief Space-separated uppercase hex bytes, empty for remote frames.
QString formatData(CanFrame const & frame);
