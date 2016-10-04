/* 
 * File:   CGameStateManager.cpp
 * Author: sheldor
 * 
 * Created on 17 de marzo de 2014, 21:08
 */

#include "CGameStateManager.hpp"

using namespace game::game_states;

CGameStateManager::CGameStateManager(CGameContainer *gc)
{
    this->current_state = NULL;
    
    this->gc = gc;
    
    this->set_state(game::game_states::states::PLAY);
}

CGameStateManager::CGameStateManager(const CGameStateManager& orig)
{
}

CGameStateManager::~CGameStateManager()
{
}


void CGameStateManager::update(const float dt)
{
    //Event handling
    this->current_state->events();
    //updating
    int state = this->current_state->update(dt);
    
    if (state != states::NULLSTATE)
    {
        this->set_state(state);
    }
    
}


void CGameStateManager::set_state(int id)
{
    if (this->current_state != NULL)
    {
        this->current_state->clear();
        delete this->current_state;
    }
    
    switch(id)
    {
        case states::MAP:
            this->current_state = new CMapState(this->gc);
            break;
        case states::PLAY:
            this->current_state = new CPlayState(this->gc);
            break;
        /*case ID::MENU:
            this->current_state = new CMenuState(this->gc);
            break;        
        case ID::MAP:
            this->current_state = new CMapState(this->gc);
            break;*/
    }
    
    this->current_state->init();
}

void CGameStateManager::render()
{
    //Rendering
    this->current_state->render();
}