#pragma once

#include <cstdint>

namespace engine
{

struct Color
{
    std::uint8_t r{};
    std::uint8_t g{};
    std::uint8_t b{};
    std::uint8_t a{255};

    static const Color Black;
    static const Color White;
};

inline constexpr Color Color::Black{0, 0, 0, 255};
inline constexpr Color Color::White{255, 255, 255, 255};

} // namespace engine
