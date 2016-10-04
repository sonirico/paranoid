/* 
 * File:   CPlayState.hpp
 * Author: sheldor
 *
 * Created on 17 de marzo de 2014, 21:11
 */

#ifndef CPLAYSTATE_HPP
#define	CPLAYSTATE_HPP

#include "assets.h"
#include "CState.hpp"
#include "CResourceHolder.hpp"
#include "CPaddle.hpp"
#include "CBall.hpp"
#include "CBrick.hpp"
#include "CBonus.hpp"

class CPlayState : public CState
{
    public:
        
        CPlayState(CGameContainer *gc);
        CPlayState(const CPlayState& orig);
        virtual ~CPlayState();
        
        void init();
        void events();
        int  update(float dt);
        void render();
        void clear();
        
        void load_bricks();
        void load_textures();
        
        void update_balls (const float dt);
        bool update_bricks(const float dt);
        void update_paddle(const float dt);
        void update_bonus (const float dt);
        
        void render_balls();
        void render_bricks();
        void render_paddle();
        void render_bonus();
        
        void clear_bricks();
        void clear_balls();
        void clear_bonus();
        
        void next_stage();
        
        void insert_bonus(CBrick *b);
        
        static unsigned int get_current_state();
        
    private:

        CPaddle *paddle;
        
        typedef std::list<CBall*> ball_list;
        typedef std::list<CBall*>::iterator ball_iterator;
        
        ball_list *balls;
        ball_iterator *balls_iterator;
        
        typedef std::list<CBrick*> brick_list;
        typedef std::list<CBrick*>::iterator brick_iterator;
        
        brick_list *bricks;
        brick_iterator *bricks_iterator;
        
        typedef std::list<CBonus*> bonus_list;
        typedef std::list<CBonus*>::iterator bonuss_iterator;
        
        bonus_list *bonus;
        bonuss_iterator *bonus_iterator;
        
        static unsigned int current_stage;
        unsigned int total_bricks;
};

#endif	/* CPLAYSTATE_HPP */

