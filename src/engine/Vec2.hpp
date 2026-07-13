#pragma once

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
