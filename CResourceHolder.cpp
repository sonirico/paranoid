/* 
 * File:   CResourceHolder.cpp
 * Author: sheldor
 * 
 * Created on 17 de marzo de 2014, 21:35
 */

#include <stdexcept>

#include "CResourceHolder.hpp"

CResourceHolder::CResourceHolder()
{
}

CResourceHolder::CResourceHolder(const CResourceHolder& orig)
{
}

CResourceHolder::~CResourceHolder()
{
}

void CResourceHolder::load
(
    game::game_textures::textures id,     
    const std::string& file_name
)
{
    std::unique_ptr<sf::Texture> texture (new sf::Texture());
    
    if (!texture->loadFromFile(file_name))
    {
        throw std::runtime_error("CResourceHolder::load. Failed to load " + file_name);
    }
    
    auto inserted = this->texture_map.insert
    ( 
        std::make_pair(id, std::move(texture) ) 
    );
    
    assert(inserted.second);
}

sf::Texture& CResourceHolder::get(game::game_textures::textures id)
{
    auto found = this->texture_map.find(id);
    //Al parecer todo a ido bien si es distino del final del mapa :/
    assert(found != this->texture_map.end());
    //retorna algo como un array[2], donde 0 es id y 1 la fuente
    return *found->second;
}

const sf::Texture& CResourceHolder::get(game::game_textures::textures id) const
{
    auto found = this->texture_map.find(id);
    //Al parecer todo a ido bien si es distino del final del mapa :/
    assert(found != this->texture_map.end());
    //retorna algo como un array[2], donde 0 es id y 1 la fuente
    return *found->second;
}

//Sonidos


void CResourceHolder::load(game::game_fx::fx id, const std::string& file_name)
{
    std::unique_ptr<sf::SoundBuffer> ffxx (new sf::SoundBuffer());
    
    if (!ffxx->loadFromFile(file_name))
    {
        throw std::runtime_error("CResourceHolder::load. Failed to load " + file_name);
    }
    
   
    auto inserted = this->fx_map.insert
    ( 
        std::make_pair(id, std::move(ffxx) ) 
    );
    
    
    assert(inserted.second);
}

sf::SoundBuffer& CResourceHolder::get(game::game_fx::fx id)
{
    auto found = this->fx_map.find(id);
    //Al parecer todo a ido bien si es distino del final del mapa :/
    assert(found != this->fx_map.end());
    //retorna algo como un array[2], donde 0 es id y 1 la fuente
    return *found->second;
}

const sf::SoundBuffer& CResourceHolder::get(game::game_fx::fx id) const
{
    auto found = this->fx_map.find(id);
    //Al parecer todo a ido bien si es distino del final del mapa :/
    assert(found != this->fx_map.end());
    //retorna algo como un array[2], donde 0 es id y 1 la fuente
    return *found->second;
}