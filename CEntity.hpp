/* 
 * File:   CEntity.hpp
 * Author: sheldor
 *
 * Created on 22 de marzo de 2014, 1:08
 */

#ifndef CENTITY_HPP
#define	CENTITY_HPP

#include <list>

#include "assets.h"
#include "CResourceHolder.hpp"
#include "CState.hpp"
#include "AnimatedSprite.hpp"

class CEntity :
        public sf::Transformable,
        public sf::Drawable
{
    public:
        
        virtual void init() = 0;
        virtual void update(const float dt) = 0;
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const = 0;
        virtual void reset() = 0;
        
        sf::FloatRect get_bounds();
        
        void stop_animation();
        void play_animation();
        
        void set_id(unsigned int id);
        int  get_id();
        
        sf::Vector2f get_velocity();
        
        
    protected:
        
        Animation *current_animation;
        AnimatedSprite animated_sprite;
        
        unsigned int id;
                
        std::list<unsigned int> children_id;
                
        sf::Vector2f velocity;
        
        sf::Vector2f scalation {2, 2};
        
        sf::Vector2f bounds;
        
        CState *state;       
};

#endif	/* CENTITY_HPP */

