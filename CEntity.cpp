
#include "CEntity.hpp"

void CEntity::set_id(unsigned int id)
{
    this->id = id;
}

int CEntity::get_id()
{
    return this->id;
}

void CEntity::stop_animation()
{
    this->animated_sprite.stop();
}

void CEntity::play_animation()
{
    this->animated_sprite.play();
}

sf::FloatRect CEntity::get_bounds()
{
    return this->animated_sprite.getGlobalBounds();
}

sf::Vector2f CEntity::get_velocity()
{
    return this->velocity;
}
