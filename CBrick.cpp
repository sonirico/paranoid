/* 
 * File:   CBrick.cpp
 * Author: sheldor
 * 
 * Created on 4 de abril de 2014, 21:50
 */

#include "CBrick.hpp"
#include "Animation.hpp"
#include "CPlayState.hpp"

using namespace game::game_bricks;

CBrick::CBrick(CState *st, bricks t) : 
    lifes(0),
    type(t)
{
    this->state = st;
    
    this->init();
}

CBrick::CBrick(const CBrick& orig)
{
}

CBrick::~CBrick()
{
    delete this->current_animation;
}

void CBrick::init()
{
    this->settings();
    
    this->scale(this->scalation.x, this->scalation.y);
     
    this->bounds.x = this->animated_sprite.getGlobalBounds().width;
    this->bounds.y = this->animated_sprite.getGlobalBounds().height;
    
    this->bounds.x *= this->scalation.x;
    this->bounds.y *= this->scalation.y;
}

void CBrick::update(const float dt)
{
    this->animated_sprite.update(sf::seconds(dt));
   // this->animated_sprite.play();
}

void CBrick::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= this->getTransform();
    
    target.draw(this->animated_sprite, states);    
}

void CBrick::reset()
{
}

void CBrick::settings()
{
    this->rects = std::vector<sf::IntRect>();
    
    unsigned int w = BRICK_BOUND.x;
    unsigned int h = BRICK_BOUND.y;
    
    switch(this->type)
    {
        case bricks::RED:
            this->score = 90;
            this->lifes = 1;
            this->rects.push_back(sf::IntRect(64, 0, w, h));
            break;
        case bricks::AQUA:
            this->score = 70;
            this->lifes = 1;
            this->rects.push_back(sf::IntRect(64 + w * 2, 0, w, h));
            break;
        case bricks::BLUE:
            this->score = 100;
            this->lifes = 1;
            this->rects.push_back(sf::IntRect(64 + w * 2, h, w, h));
            break;
        case bricks::GREEN:
            this->score = 80;
            this->lifes = 1;
            this->rects.push_back(sf::IntRect(64 + w, h, w, h));
            break;
        case bricks::ORANGE:
            this->score = 60;
            this->lifes = 1;
            this->rects.push_back(sf::IntRect(64, h, w, h));
            break;
        case bricks::PURPPLE:
            this->score = 110;
            this->lifes = 1;
            this->rects.push_back(sf::IntRect(64 + w * 3, 0, w, h));
            break;
        case bricks::WHITE:
            this->score = 50;
            this->lifes = 1;
            this->rects.push_back(sf::IntRect(64 + w * 3, h, w, h));
            break;
        case bricks::YELLOW:
            this->score = 120;
            this->lifes = 1;
            this->rects.push_back(sf::IntRect(64 + w, 0, w, h));
            break;
        case bricks::SILVER:
            this->score = 50 * CPlayState::get_current_state();
            this->lifes = 2;
            for(unsigned int i = 0; i < 6; i++)
                this->rects.push_back(sf::IntRect(64 + w * 4, h * i, w, h));
            break;
        case bricks::GOLD:
            this->score = 100 * CPlayState::get_current_state();
            this->lifes = 3;
            for(unsigned int i = 0; i < 6; i++)
                this->rects.push_back(sf::IntRect(64 + w * 5, h * i, w, h));
            break;
        case bricks::UNDESTROYABLE:
            this->score = 0;
            for(unsigned int i = 0; i < 6; i++)
                this->rects.push_back(sf::IntRect(64 + w * 6, h * i, w, h));
            break;            
    }//End switch
    
    this->current_animation = new Animation();
    
    this->current_animation->setSpriteSheet(this->state->rh->get(game::game_textures::textures::MAIN));
    
    for(unsigned int i = 0; i < this->rects.size(); i++)
        this->current_animation->addFrame(this->rects[i]);
    
    this->animated_sprite = AnimatedSprite(sf::seconds(0.05), true, false);
    this->animated_sprite.play(*(this->current_animation));
    
}//End settings

void CBrick::quit_life()
{
    if (this->lifes > 0)
    {
        this->lifes--;
        this->removable = this->lifes <= 0;
    }
}


bool CBrick::is_removable()
{
    return this->removable;
}