#pragma once

#include "engine/Vec2.hpp"

#include <string>

struct SDL_Renderer;
struct SDL_Texture;

namespace engine
{

// Owning wrapper around an SDL GPU texture. Loads with nearest-neighbour
// scaling so the pixel art stays crisp when scaled up.
class Texture
{
  public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    bool loadFromFile(SDL_Renderer* renderer, const std::string& path);

    Vec2i getSize() const;
    SDL_Texture* get() const;

  private:
    SDL_Texture* m_handle = nullptr;
    Vec2i m_size;
};

} // namespace engine
