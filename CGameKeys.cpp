/* 
 * File:   GameKeys.cpp
 * Author: sheldor
 * 
 * Created on 2 de abril de 2014, 21:38
 */

#include "CGameKeys.hpp"

using namespace game::game_keys;

std::string CGameKeys::text_entered;
const unsigned int CGameKeys::LENGTH;
bool CGameKeys::game_keys[];
bool CGameKeys::previous_game_keys[];

void CGameKeys::update_keys()
{
    for(unsigned int i = 0; i < LENGTH; i++)
    {
        previous_game_keys[i] = game_keys[i];
    }
}

void CGameKeys::reset_keys()
{    
    for(unsigned int i = 0; i < LENGTH; i++)
    {
        game_keys[i] = false;
        previous_game_keys[i] = false;
    }
}

void CGameKeys::set_key(keys k, bool b)
{
    unsigned int i = static_cast<int>(k);
    game_keys[i] = b;
    std::cout << "key: " << i << ", " << b << "\n";
}

void CGameKeys::set_text(std::string text)
{
    CGameKeys::text_entered = text;
}

bool CGameKeys::is_key_pressed(keys k)
{
    unsigned int i = static_cast<int>(k);
    return game_keys[i] && !previous_game_keys[i];
}

bool CGameKeys::is_key_down(keys k)
{
    unsigned int i = static_cast<int>(k);
    return game_keys[i];
}

std::string CGameKeys::get_text_entered()
{
    return text_entered;
}