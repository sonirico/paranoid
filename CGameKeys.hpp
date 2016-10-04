/* 
 * File:   CGameKeys.hpp
 * Author: sheldor
 *
 * Created on 2 de abril de 2014, 23:09
 */

#ifndef CGAMEKEYS_HPP
#define	CGAMEKEYS_HPP

#include <string>

#include "assets.h"

using namespace game::game_keys;

class CGameKeys
{
    private:
        
        static std::string text_entered;

        static const unsigned int LENGTH = static_cast<int>(keys::COUNT);

        static bool game_keys [LENGTH];
        static bool previous_game_keys[LENGTH];
        // --------- Funciones ------------
    public:
        
        static void update_keys();

        static void set_key(keys i, bool b);

        static void set_text(std::string text);

        static bool is_key_pressed(keys i);

        static bool is_key_down(keys i);

        static std::string get_text_entered();

        static void reset_keys();
};

#endif	/* CGAMEKEYS_HPP */

