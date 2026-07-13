#include "CEntity.hpp"

#include "engine/Window.hpp"

void CEntity::draw(engine::Window& target) const
{
    engine::AnimatedSprite sprite = this->animated_sprite;

    sprite.setPosition(this->getPosition());
    sprite.setScale(this->getScale());

    target.draw(sprite);
}

engine::FloatRect CEntity::get_bounds() const
{
    return this->animated_sprite.getGlobalBounds();
}

engine::Vec2f CEntity::get_size() const
{
    return this->bounds;
}

void CEntity::stop_animation()
{
    this->animated_sprite.stop();
}

void CEntity::play_animation()
{
    this->animated_sprite.play();
}

void CEntity::set_id(unsigned int id)
{
    this->id = id;
}

int CEntity::get_id() const
{
    return this->id;
}

engine::Vec2f CEntity::get_velocity() const
{
    return this->velocity;
}
