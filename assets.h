/* 
 * File:   assets.h
 * Author: sheldor
 *
 * Created on 22 de marzo de 2014, 0:56
 */

#ifndef ASSETS_H
#define	ASSETS_H

#include <iostream>
#include <string>

#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace game
{
    const unsigned int WIDTH = 800;
    const unsigned int HEIGHT = 600;
    const std::string TITLE = "PARANOID";

    const unsigned int FRAMES = 60;
    const sf::Time TIME_PER_FRAME = sf::seconds(1.f / FRAMES);
    
    namespace game_states
    {
        enum states
        {
            NULLSTATE = 0, MENU, PLAY, MAP
        };
    };
    
    namespace game_textures
    {
        enum textures
        {
            BACKGROUND, MAIN, BRICKS
        };
    };
    
    namespace game_bonus
    {
        enum bonus
        {
            B = 0, C, D, E, L, M, N, P, R, S, T, X, COUNT
        };
    };
    
    namespace game_bricks
    {
        enum bricks
        {
            NONE = 0, RED, ORANGE, YELLOW, GREEN, AQUA, BLUE, PURPPLE, WHITE, SILVER, GOLD, UNDESTROYABLE, COUNT
        };
        
        const unsigned int TOTAL_STAGES = 10;
        const unsigned int BRICKS_PER_LINE = 15;
        const unsigned int LINES = 15;
        const sf::Vector2f BRICK_BOUND = sf::Vector2f(16, 8);
        const unsigned int MARGIN_LEFT = (game::WIDTH - (BRICKS_PER_LINE * BRICK_BOUND.x * 2)) / 2;
        const unsigned int MARGIN_TOP = 100;
        
        const unsigned int brick_map [TOTAL_STAGES][BRICKS_PER_LINE * LINES] = 
        {
            //Stage 1 -> Dragon Ball
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,0,2,2,2,2,2,2,1,2,1,2,2,2,2,2,2,2,2,2,2,2,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,1,2,2,2,2,2,2,0,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            //Stage 2 -> ParseCode
            {0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,11,11,11,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,6,6,6,6,6,6,6,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,0,0,0,0,0,6,6,6,8,8,8,8,8,8,6,6,0,0,0,0,6,6,6,8,6,8,6,6,6,6,6,0,0,0,0,6,6,6,8,8,8,6,6,6,6,6,0,0,0,0,0,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,6,6,6,6,6,6,6,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,11,11,11,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0},
            //Stage 3 -> Escalera
            {6,6,6,6,6,6,6,6,6,6,6,6,6,6,9,0,5,5,5,5,5,5,5,5,5,5,5,5,5,9,0,0,4,4,4,4,4,4,4,4,4,4,4,4,9,0,0,0,3,3,3,3,3,3,3,3,3,3,3,9,0,0,0,0,2,2,2,2,2,2,2,2,2,2,9,0,0,0,0,0,1,1,1,1,1,1,1,1,1,9,0,0,0,0,0,0,8,8,8,8,8,8,8,8,9,0,0,0,0,0,0,0,7,7,7,7,7,7,7,9,0,0,0,0,0,0,0,0,6,6,6,6,6,6,9,0,0,0,0,0,0,0,0,0,5,5,5,5,5,9,0,0,0,0,0,0,0,0,0,0,4,4,4,4,9,0,0,0,0,0,0,0,0,0,0,0,3,3,3,9,0,0,0,0,0,0,0,0,0,0,0,0,2,2,9,0,0,0,0,0,0,0,0,0,0,0,0,0,1,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
            //Stage 4 -> PACO
            //{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},
            //Stage 5 escalera invertida
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,11,11,11,11,11,11,11,11,11,11,11,2,0,0,3,3,3,10,10,10,10,3,3,3,3,2,0,0,0,11,11,11,11,11,11,11,11,11,11,2,0,0,0,0,3,3,3,10,10,10,3,3,3,2,0,0,0,0,0,11,11,11,11,11,11,11,11,2,0,0,0,0,0,0,3,3,3,10,10,3,3,2,0,0,0,0,0,0,0,11,11,11,11,11,11,2,0,0,0,0,0,0,0,0,3,3,3,10,3,2,0,0,0,0,0,0,0,0,0,11,11,11,11,2,0,0,0,0,0,0,0,0,0,0,3,3,3,2,0,0,0,0,0,0,0,0,0,0,0,11,11,2,0,0,0,0,0,0,0,0,0,0,0,0,3,2,0,0,0,5,6,8,8,8,8,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
            {},
            {},
            {},
            {},
            {}
        };
    };
    
    namespace game_keys
    {
        enum keys
        {
            LEFT = 0, RIGHT, SPACE, ENTER, ESCAPE, ANY, COUNT//El total
        };
    };
    
    namespace game_fx
    {
        enum fx
        {
            REBOTE1 = 0, REBOTE2
        };
    };
    
};



#endif	/* ASSETS_H */
