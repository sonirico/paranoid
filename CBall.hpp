/* 
 * File:   CBall.hpp
 * Author: sheldor
 *
 * Created on 3 de abril de 2014, 1:03
 */

#ifndef CBALL_HPP
#define	CBALL_HPP

#include "assets.h"
#include "CEntity.hpp"
#include "CPaddle.hpp"
#include "CBrick.hpp"

class CPaddle;//Sirve para gestionar la depencia circular CBall <--> CPaddle

class CBall : public CEntity
{
    public:
        
        CBall(CState *st, CPaddle *pd);
        CBall(const CBall& orig);
        virtual ~CBall();
        
    public:

        void init();
        void update(const float dt);
        void draw(sf::RenderTarget &target, sf::RenderStates states) const;
        void reset();
        /*
        void set_id(unsigned int id);
        int  get_id();*/
        void set_in_paddle();
        void set_in_paddle(bool b);
        
        bool is_in_paddle();
        bool is_removable();
        bool is_collisionable();
        
        bool collision_ball_brick(CBrick *b, const float dt);
        
    private:
        
        void collision_ball_paddle(const float dt);
        
        void check_bounds();
        
        bool check_point_rect(sf::Vector2f point, CBrick *b);
        bool check_collision(CBrick *b);
        
        CPaddle *paddle;
        
        bool in_paddle;
        
        bool removable{false};
        
        int delta_vel{10};
        
        int limit_vel{300};
        
        int vel{400};
        
        bool dirx{false};
        bool diry{false};
        
};

#endif	/* CBALL_HPP */

