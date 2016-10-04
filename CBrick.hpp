/* 
 * File:   CBrick.hpp
 * Author: sheldor
 *
 * Created on 4 de abril de 2014, 21:50
 */

#ifndef CBRICK_HPP
#define	CBRICK_HPP

#include "assets.h"
#include "CEntity.hpp"
#include "CState.hpp"

class CBrick : public CEntity
{
    public:

        CBrick(CState *state, game::game_bricks::bricks t);
        CBrick(const CBrick& orig);
        virtual ~CBrick();
    
    public:

        void init();
        void update(const float dt);
        void draw(sf::RenderTarget &target, sf::RenderStates states) const;
        void reset();
        
        void quit_life();
        
        bool is_removable();
    
    public:
        game::game_bricks::bricks type;
  
    private:
        
        void settings();
        
        unsigned int lifes;
        unsigned int score;
        
        bool removable{false};
        
        std::vector<sf::IntRect> rects;
};

#endif	/* CBRICK_HPP */

