/* 
 * File:   CState.hpp
 * Author: sheldor
 *
 * Created on 17 de marzo de 2014, 21:07
 */

#ifndef CSTATE_HPP
#define	CSTATE_HPP

#include "assets.h"
#include "CGameContainer.hpp"
#include "CResourceHolder.hpp"

class CState
{
    public:
        virtual void init() = 0;
        virtual void events() = 0;
        virtual int update(const float dt) = 0;
        virtual void render() = 0;
        virtual void clear() = 0;
        
        virtual void load_textures() = 0;
        
    public:
        
        CGameContainer *gc;        
        CResourceHolder *rh;
        
};

#endif	/* CSTATE_HPP */

