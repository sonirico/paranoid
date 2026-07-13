#include "engine/Animation.hpp"

namespace engine
{

void Animation::addFrame(const IntRect& rect)
{
    m_frames.push_back(rect);
}

void Animation::setSpriteSheet(const Texture& texture)
{
    m_texture = &texture;
}

const Texture* Animation::getSpriteSheet() const
{
    return m_texture;
}

std::size_t Animation::getSize() const
{
    return m_frames.size();
}

const IntRect& Animation::getFrame(std::size_t n) const
{
    return m_frames[n];
}

} // namespace engine
