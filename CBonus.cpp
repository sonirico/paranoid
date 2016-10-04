/* 
 * File:   CBonus.cpp
 * Author: sheldor
 * 
 * Created on 10 de abril de 2014, 17:06
 */

#include "CBonus.hpp"
#include "CBall.hpp"

CBonus::CBonus(CState *state, game::game_bonus::bonus t)
{
    this->state = state;
    this->type = t;
    
    this->init();
}

CBonus::CBonus(const CBonus& orig)
{
}

CBonus::~CBonus()
{
    delete this->current_animation;
}

void CBonus::init()
{
    this->settings();
    
    this->bounds.x = this->animated_sprite.getGlobalBounds().width * this->scalation.x;
    this->bounds.y = this->animated_sprite.getGlobalBounds().height * this->scalation.y;
    
    this->velocity = sf::Vector2f(0, 100);
    
    this->scale(this->scalation);
}

void CBonus::update(const float dt)
{
    this->move(this->velocity * dt);
    this->animated_sprite.update(sf::seconds(dt));
}

void CBonus::reset()
{
}

void CBonus::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= this->getTransform();
    
    target.draw(this->animated_sprite, states);
}

void CBonus::set_position(sf::Vector2f pos)
{
    this->setPosition(pos);
    //this->setPosition(b->getPosition());
}

void CBonus::settings()
{
    using namespace game::game_bonus;
    using namespace game::game_textures;
    
    this->rects = std::vector<sf::IntRect>();
    
    unsigned int w = 16;//this->bounds.x / this->scalation.x;
    unsigned int h = 8;//this->bounds.y / this->scalation.y;
    
    switch(this->type)
    {
        case bonus::B:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176, h * i, w, h));
            break;
        case bonus::C:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w, h * i, w, h));
            break;
        case bonus::D:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 2, h * i, w, h));
            break;
        case bonus::E:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 3, h * i, w, h));
            break;
        case bonus::L:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 4, h * i, w, h));
            break;
        case bonus::M:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 5, h * i, w, h));
            break;
        case bonus::N:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 6, h * i, w, h));
            break;
        case bonus::P:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 7, h * i, w, h));
            break;
        case bonus::R:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 8, h * i, w, h));
            break;
        case bonus::S:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 9, h * i, w, h));
            break;
        case bonus::T:
            for (unsigned int i = 0; i < 7; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 10, h * i, w, h));
            break;
        case bonus::X:
            for (unsigned int i = 0; i < 6; ++i)
                this->rects.push_back(sf::IntRect(176 + w * 11, 16 * i, 16, 16));
            break;
    }
    
    this->current_animation = new Animation();
    this->current_animation->setSpriteSheet(this->state->rh->get(textures::MAIN));
    
    for (unsigned int i = 0; i < this->rects.size(); ++i)
        this->current_animation->addFrame(this->rects[i]);
    
    this->animated_sprite = AnimatedSprite(sf::seconds(0.1f), false, true);
    this->animated_sprite.play(*(this->current_animation));
}

bool CBonus::collision_bonus_paddle(CPaddle* p)
{
    float x1, y1, w1, h1;
    float x2, y2, w2, h2;
    
    x1 = this->getPosition().x;
    y1 = this->getPosition().y;
    w1 = this->bounds.x;
    h1 = this->bounds.y;
    
    x2 = p->getPosition().x;
    y2 = p->getPosition().y;
    w2 = p->get_bounds().width * 2;
    h2 = p->get_bounds().height * 2;
    
    if ( (x1 + w1) < x2 ) return false;
    if ( (y1 + h1) < y2 ) return false;
    if ( (x2 + w2) < x1 ) return false;
    if ( (y2 + h2) < y1 ) return false;
    
    this->removable = true;
    
    return true;
}

bool CBonus::is_removable()
{
    return this->removable;
}