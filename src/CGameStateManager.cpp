#include "CGameStateManager.hpp"

#include "CMapState.hpp"
#include "CMenuState.hpp"
#include "CPlayState.hpp"

using namespace game::game_states;

CGameStateManager::CGameStateManager(CGameContainer* gc, int initial_state)
{
    this->gc = gc;

    this->set_state(initial_state);
}

void CGameStateManager::update(const float dt)
{
    this->current_state->events();

    int state = this->current_state->update(dt);

    if (state != NULLSTATE)
    {
        this->set_state(state);
    }
}

void CGameStateManager::set_state(int id)
{
    if (this->current_state)
    {
        this->current_state->clear();
    }

    switch (id)
    {
    case MENU:
        this->current_state = std::make_unique<CMenuState>(this->gc);
        break;
    case MAP:
        this->current_state = std::make_unique<CMapState>(this->gc);
        break;
    case PLAY:
        this->current_state = std::make_unique<CPlayState>(this->gc);
        break;
    }

    this->current_state->init();
}

void CGameStateManager::render()
{
    this->current_state->render();
}

CState* CGameStateManager::getCurrentState() const
{
    return this->current_state.get();
}
