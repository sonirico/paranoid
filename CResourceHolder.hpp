/* 
 * File:   CResourceHolder.hpp
 * Author: sheldor
 *
 * Created on 17 de marzo de 2014, 21:35
 */

#ifndef CRESOURCEHOLDER_HPP
#define	CRESOURCEHOLDER_HPP

#include <map>
#include <memory>
#include <algorithm>
#include <cassert>

#include "assets.h"

class CResourceHolder
{
    public:
        
        CResourceHolder();
        CResourceHolder(const CResourceHolder& orig);
        virtual ~CResourceHolder();
        
    public:
        //Texturas
        void load( game::game_textures::textures id, const std::string &file_name );
        sf::Texture& get(game::game_textures::textures id);
        const sf::Texture& get(game::game_textures::textures id) const;
        //Efectos de sonido
        void load( game::game_fx::fx id, const std::string &file_name );
        sf::SoundBuffer& get(game::game_fx::fx id);
        const sf::SoundBuffer& get(game::game_fx::fx id) const;
        
    private:
        
        //Creamos el mapa de texturas id -> fuente
        std::map < game::game_textures::textures, std::unique_ptr<sf::Texture> > texture_map;
        // Mapa de audios
        std::map < game::game_fx::fx, std::unique_ptr<sf::SoundBuffer> > fx_map;
};

#endif	/* CRESOURCEHOLDER_HPP */

