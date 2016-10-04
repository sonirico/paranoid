/* 
 * File:   CGameContainer.cpp
 * Author: sheldor
 * 
 * Created on 25 de marzo de 2014, 23:19
 */

#include "CGameContainer.hpp"

using namespace game::game_keys;

CGameContainer::CGameContainer()
{
    this->window = new sf::RenderWindow
    (
        sf::VideoMode(game::WIDTH, game::HEIGHT),
        game::TITLE
    );//, sf::Style::Fullscreen);
    this->rh = new CResourceHolder();
    //CGameKeys::reset_keys();//reseteamos los
    this->init();
}

CGameContainer::CGameContainer(const CGameContainer& orig)
{
}

CGameContainer::~CGameContainer()
{
    delete this->window;
    delete this->rh;
}

void CGameContainer::init()
{
    this->current_sound[0] = sf::Sound();
    this->current_sound[1] = sf::Sound();
    
    this->rh->load(game::game_fx::fx::REBOTE1, "media/fx/rebote1.wav");
    this->rh->load(game::game_fx::fx::REBOTE2, "media/fx/rebote2.wav");
    
    this->current_sound[0].setBuffer(this->rh->get(game::game_fx::fx::REBOTE1));
    this->current_sound[1].setBuffer(this->rh->get(game::game_fx::fx::REBOTE2));
}

void CGameContainer::events()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        this->window->close();
    //CGameKeys::update_keys();
    // Aqui comprobamos si queremos salir del juego Y YA 
    sf::Event event;    
    
    while (this->window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            this->window->close();
            break;
        }
    }
}


void CGameContainer::play_fx(game::game_fx::fx id)
{
    /*std::cout << id << std::endl;
    sf::Sound fx;
    fx.setBuffer(this->rh->get(id));
    fx.play();*/
    int i = static_cast<int>(id);
    this->current_sound[i].setVolume(10);
    this->current_sound[i].play();
    
}