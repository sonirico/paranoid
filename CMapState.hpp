/* 
 * File:   CMapState.hpp
 * Author: sheldor
 *
 * Created on 17 de marzo de 2014, 21:12
 */

#ifndef CMAPSTATE_HPP
#define	CMAPSTATE_HPP

#include "assets.h"
#include "CState.hpp"
#include "CResourceHolder.hpp"
#include "CPaddle.hpp"
#include "CBall.hpp"
#include "CBrick.hpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <list>

struct SSelector;

class CMapState : public CState
{
    public:
        
        CMapState(CGameContainer *gc);
        CMapState(const CMapState& orig);
        virtual ~CMapState();
        
    private:
        
        void init();
        void events();
        int  update(float dt);
        void render();
        void clear();
        
        void load_textures();
        
        void update_balls (const float dt);
        void update_bricks(const float dt);
        void update_paddle(const float dt);        
        
        void render_balls();
        void render_bricks();
        void render_paddle();
        
        void clear_bricks();
        void clear_balls();
                
    private:
        
        void get_map();
        
        
    private:
        
        unsigned int last_id;
        //Array bidimensional c++
        int *map;
        
        CPaddle *paddle;
        
        typedef std::list<CBall*> ball_list;
        typedef std::list<CBall*>::iterator ball_iterator;
        
        ball_list *balls;
        ball_iterator *balls_iterator;
        
        typedef std::list<CBrick*> brick_list;
        typedef std::list<CBrick*>::iterator brick_iterator;
        
        brick_list *bricks;
        brick_iterator *bricks_iterator;
        
        SSelector *selector;
};

#endif	/* CMAPSTATE_HPP */

