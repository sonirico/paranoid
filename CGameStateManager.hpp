/* 
 * File:   CGameStateManager.hpp
 * Author: sheldor
 *
 * Created on 17 de marzo de 2014, 21:08
 */

#ifndef CGAMESTATEMANAGER_HPP
#define	CGAMESTATEMANAGER_HPP

#include "assets.h"

#include "CGameContainer.hpp"
#include "CResourceHolder.hpp"

#include "CState.hpp"

#include "CMapState.hpp"
#include "CMenuState.hpp"
#include "CPlayState.hpp"

class CGameStateManager
{
    public:
        
        CGameStateManager(CGameContainer *gc);
        CGameStateManager(const CGameStateManager& orig);
        virtual ~CGameStateManager();

    public:

        void update(const float dt);
        void render();
        
    private:
        
        void set_state(int state_id);
        
    private:
        
        CGameContainer *gc;
        CResourceHolder *rh;
        
        CState *current_state;

};

#endif	/* CGAMESTATEMANAGER_HPP */

