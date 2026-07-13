#pragma once

#include "engine/Rect.hpp"

#include <cstddef>
#include <vector>

namespace engine
{

class Texture;

// An ordered list of frames (sub-rects) over one sprite sheet.
class Animation
{
  public:
    Animation() = default;

    void addFrame(const IntRect& rect);

    void setSpriteSheet(const Texture& texture);
    const Texture* getSpriteSheet() const;

    std::size_t getSize() const;
    const IntRect& getFrame(std::size_t n) const;

  private:
    std::vector<IntRect> m_frames;
    const Texture* m_texture = nullptr;
};

} // namespace engine
