#include "CResourceHolder.hpp"

#include <cassert>
#include <stdexcept>

CResourceHolder::CResourceHolder(SDL_Renderer* renderer) : renderer(renderer) {}

void CResourceHolder::load(game::game_textures::textures id, const std::string& file_name)
{
    auto texture = std::make_unique<engine::Texture>();

    if (!texture->loadFromFile(this->renderer, file_name))
    {
        throw std::runtime_error("CResourceHolder::load. Failed to load " + file_name);
    }

    auto inserted = this->texture_map.insert(std::make_pair(id, std::move(texture)));

    assert(inserted.second);
}

engine::Texture& CResourceHolder::get(game::game_textures::textures id)
{
    auto found = this->texture_map.find(id);
    assert(found != this->texture_map.end());

    return *found->second;
}

const engine::Texture& CResourceHolder::get(game::game_textures::textures id) const
{
    auto found = this->texture_map.find(id);
    assert(found != this->texture_map.end());

    return *found->second;
}

void CResourceHolder::load(game::game_fx::fx id, const std::string& file_name)
{
    auto buffer = std::make_unique<engine::SoundBuffer>();

    if (!buffer->loadFromFile(file_name))
    {
        throw std::runtime_error("CResourceHolder::load. Failed to load " + file_name);
    }

    auto inserted = this->fx_map.insert(std::make_pair(id, std::move(buffer)));

    assert(inserted.second);
}

engine::SoundBuffer& CResourceHolder::get(game::game_fx::fx id)
{
    auto found = this->fx_map.find(id);
    assert(found != this->fx_map.end());

    return *found->second;
}

const engine::SoundBuffer& CResourceHolder::get(game::game_fx::fx id) const
{
    auto found = this->fx_map.find(id);
    assert(found != this->fx_map.end());

    return *found->second;
}
