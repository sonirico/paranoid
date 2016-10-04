/* 
 * File:   CPaddle.cpp
 * Author: sheldor
 * 
 * Created on 17 de marzo de 2014, 21:09
 */

#include "CPaddle.hpp"

CPaddle::CPaddle(CState *st)
{
    this->state = st;
    
    this->init();    
}

CPaddle::CPaddle(const CPaddle& orig)
{
}

CPaddle::~CPaddle()
{
    delete this->current_animation;
    //se puede->  delete this->state;
}

void CPaddle::init()
{
    this->velocity.x = 400;
    this->velocity.y = 0;
    
    this->scale(this->scalation.x, this->scalation.y);
    
    this->rects.push_back(sf::IntRect(128, 72, 32, 8));
    this->rects.push_back(sf::IntRect(128, 80, 32, 8));
    this->rects.push_back(sf::IntRect(128, 88, 32, 8));
    this->rects.push_back(sf::IntRect(128, 96, 32, 8));
    this->rects.push_back(sf::IntRect(128, 88, 32, 8));
    this->rects.push_back(sf::IntRect(128, 80, 32, 8));
    
    
    this->current_animation = new Animation();
    this->current_animation->setSpriteSheet(this->state->rh->get(game::game_textures::textures::MAIN));
    
    for(unsigned int i = 0; i < this->rects.size(); ++i)
    {
        this->current_animation->addFrame(this->rects[i]);
    }
        
    this->animated_sprite = AnimatedSprite(sf::seconds(0.2), true, false);
    this->animated_sprite.play(*current_animation);
        
    this->bounds.x = this->animated_sprite.getLocalBounds().width;
    this->bounds.y = this->animated_sprite.getLocalBounds().height;
    
    this->bounds.x *= this->scalation.x;
    this->bounds.y *= this->scalation.y;
    
    //std::cout << "x: " << this->bounds.x << " y: " << this->bounds.y << std::endl;
        
    this->setPosition((game::WIDTH - this->bounds.x) / 2, game::HEIGHT - 100 );    
}

void CPaddle::update(const float dt)
{
    this->dir = 0;
    
    if( sf::Keyboard::isKeyPressed( sf::Keyboard::Right ))
    {
        this->move(this->velocity * dt);
        this->dir = 1;
    }
    if( sf::Keyboard::isKeyPressed( sf::Keyboard::Left ))
    {
        this->move(this->velocity * -dt);
        this->dir = -1;
    }    
    if (sf::Keyboard::isKeyPressed( sf::Keyboard::Space ) )
    {
        //this->unattach_ball();
    }
    
    this->animated_sprite.update(sf::seconds(dt));
    this->animated_sprite.play();
    
    this->check_bounds();   
    
    //this->move_attached_balls();
}

void CPaddle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= this->getTransform();
    
    target.draw(this->animated_sprite, states);
}

void CPaddle::reset()
{
    this->setPosition((game::WIDTH - this->bounds.x) / 2, game::HEIGHT - 100 );
}

void CPaddle::check_bounds()
{
    sf::Vector2f pos = this->getPosition();
    
    if (pos.x <= 0)
    {
        pos.x = 0;
    }
    else if ( pos.x + this->bounds.x >= game::WIDTH )
    {
        pos.x =  game::WIDTH - this->bounds.x;
    }
    
    this->setPosition(pos);
}


