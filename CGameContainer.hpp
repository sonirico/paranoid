/* 
 * File:   CGameContainer.hpp
 * Author: sheldor
 *
 * Created on 25 de marzo de 2014, 23:19
 */

#ifndef CGAMECONTAINER_HPP
#define	CGAMECONTAINER_HPP

#include "assets.h"
#include "CResourceHolder.hpp"
//#include "CGameKeys.hpp"

class CGameContainer
{
    public:
        
        CGameContainer();
        CGameContainer(const CGameContainer& orig);
        virtual ~CGameContainer();
        
        void init();
        void events();
        
        void play_music();
        void play_fx(game::game_fx::fx id);
        
    private:
        
        
    public:
        
        sf::RenderWindow *window;
        CResourceHolder *rh;
        
        sf::Sound current_sound [2];
    
};

#endif	/* CGAMECONTAINER_HPP */

