#pragma once

#include "engine/Vec2.hpp"

namespace engine
{

// Axis-aligned rectangle. Field names follow the left/top/width/height
// convention the game's collision code is written against.
template <typename T> struct Rect
{
    T left{};
    T top{};
    T width{};
    T height{};

    constexpr bool contains(const Vec2<T>& point) const
    {
        return point.x >= left && point.x < left + width && point.y >= top &&
               point.y < top + height;
    }

    constexpr bool intersects(const Rect& other) const
    {
        return left < other.left + other.width && other.left < left + width &&
               top < other.top + other.height && other.top < top + height;
    }

    constexpr bool operator==(const Rect& other) const = default;
};

using IntRect = Rect<int>;
using FloatRect = Rect<float>;

} // namespace engine
