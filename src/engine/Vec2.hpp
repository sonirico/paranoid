#pragma once

#include <cmath>

namespace engine
{

// Minimal 2D vector, the common currency for positions, velocities and sizes.
template <typename T> struct Vec2
{
    T x{};
    T y{};

    constexpr Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vec2& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr bool operator==(const Vec2& other) const = default;

    T length() const
    {
        return std::sqrt(x * x + y * y);
    }

    // The zero vector normalizes to zero instead of dividing by zero.
    Vec2 normalized() const
    {
        const T len = length();
        return len == T{} ? Vec2{} : Vec2{x / len, y / len};
    }

    // Rotated 90 degrees counter-clockwise in screen coordinates.
    constexpr Vec2 perpendicular() const
    {
        return {-y, x};
    }
};

template <typename T> constexpr Vec2<T> operator+(Vec2<T> lhs, const Vec2<T>& rhs)
{
    return lhs += rhs;
}

template <typename T> constexpr Vec2<T> operator-(Vec2<T> lhs, const Vec2<T>& rhs)
{
    return lhs -= rhs;
}

template <typename T> constexpr Vec2<T> operator*(Vec2<T> lhs, T scalar)
{
    return lhs *= scalar;
}

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<unsigned int>;

} // namespace engine
