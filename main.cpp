/* 
 * File:   main.cpp
 * Author: sheldor
 *
 * Created on 17 de marzo de 2014, 19:30
 */

#include "assets.h"

#include "CGameContainer.hpp"
#include "CGameStateManager.hpp"
#include "CResourceHolder.hpp"
#include "CPaddle.hpp"
#include "AnimatedSprite.hpp"

CResourceHolder rh = CResourceHolder();
CGameContainer gc = CGameContainer();
CGameStateManager gsm(&gc);

sf::Sprite background;

void m_load_textures();

int main(int argc, char** argv)
{
    //Cargamos las texturas
    m_load_textures();
 
    background.setTexture(rh.get(game::game_textures::textures::BACKGROUND));
  
    sf::Clock clock;
    sf::Time time_since_last_update = sf::Time::Zero;
    
    const float dt = game::TIME_PER_FRAME.asSeconds();
    
    gc.window->setFramerateLimit(game::FRAMES);
    
    while (gc.window->isOpen())
    {
        time_since_last_update += clock.restart();
        
        while( time_since_last_update > game::TIME_PER_FRAME )
        {
            time_since_last_update -= game::TIME_PER_FRAME;
            
            gc.events();//Checa si cerramos la ventana
            gsm.update(dt);//Checa eventos y actualiza dentro de los estados
        }
        
        //animatedSprite.play(*a);
        //animatedSprite.update(sf::seconds(dt));
        
        gc.window->clear();
        
        gc.window->draw(background);
        
        //gc.window->draw(animatedSprite);
        gsm.render();
        
        gc.window->display();
    }
        
    return 0;
}

void m_load_textures()
{
    using namespace game::game_textures;
    
    rh.load(textures::BACKGROUND, "media/textures/background.png");
}