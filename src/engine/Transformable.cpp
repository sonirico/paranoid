#include "engine/Transformable.hpp"

namespace engine
{

void Transformable::setPosition(const Vec2f& position)
{
    m_position = position;
}

const Vec2f& Transformable::getPosition() const
{
    return m_position;
}

void Transformable::setPosition(float x, float y)
{
    m_position = {x, y};
}

void Transformable::move(const Vec2f& offset)
{
    m_position += offset;
}

void Transformable::move(float x, float y)
{
    m_position += Vec2f{x, y};
}

void Transformable::setScale(const Vec2f& factors)
{
    m_scale = factors;
}

const Vec2f& Transformable::getScale() const
{
    return m_scale;
}

void Transformable::scale(const Vec2f& factors)
{
    m_scale = {m_scale.x * factors.x, m_scale.y * factors.y};
}

void Transformable::scale(float x, float y)
{
    scale(Vec2f{x, y});
}

void Transformable::setOrigin(const Vec2f& origin)
{
    m_origin = origin;
}

const Vec2f& Transformable::getOrigin() const
{
    return m_origin;
}

} // namespace engine
