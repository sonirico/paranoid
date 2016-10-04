/* 
 * File:   CPaddle.hpp
 * Author: sheldor
 *
 * Created on 17 de marzo de 2014, 21:09
 */

#ifndef CPADDLE_HPP
#define	CPADDLE_HPP

#include "assets.h"
#include "CEntity.hpp"
#include "CBall.hpp"
#include "AnimatedSprite.hpp"


class CBall;//Sirve para gestionar la depencia circular CBall <--> CPaddle

class CPaddle : public CEntity
{
    public:
               
        CPaddle(CState *st);
        CPaddle(const CPaddle& orig);
        virtual ~CPaddle();
        
        void init();
        void update(const float dt);
        void draw(sf::RenderTarget &target, sf::RenderStates states) const;
        void reset();
        
        void attach_ball(CBall *b);
        void unattach_ball();
        
        int dir{0};            
        
    private:
        
        void check_bounds();
        
        std::vector<sf::IntRect> rects;
        
};

#endif	/* CPADDLE_HPP */

