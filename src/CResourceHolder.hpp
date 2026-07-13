#pragma once

#include "assets.h"
#include "engine/SoundBuffer.hpp"
#include "engine/Texture.hpp"

#include <map>
#include <memory>
#include <string>

struct SDL_Renderer;

// Central owner of every texture and sound effect, keyed by asset id.
class CResourceHolder
{
  public:
    explicit CResourceHolder(SDL_Renderer* renderer);

    CResourceHolder(const CResourceHolder&) = delete;
    CResourceHolder& operator=(const CResourceHolder&) = delete;

    void load(game::game_textures::textures id, const std::string& file_name);
    engine::Texture& get(game::game_textures::textures id);
    const engine::Texture& get(game::game_textures::textures id) const;

    void load(game::game_fx::fx id, const std::string& file_name);
    engine::SoundBuffer& get(game::game_fx::fx id);
    const engine::SoundBuffer& get(game::game_fx::fx id) const;

  private:
    SDL_Renderer* renderer;

    std::map<game::game_textures::textures, std::unique_ptr<engine::Texture>> texture_map;
    std::map<game::game_fx::fx, std::unique_ptr<engine::SoundBuffer>> fx_map;
};
