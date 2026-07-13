#include "engine/AnimatedSprite.hpp"

#include <cmath>

namespace engine
{

AnimatedSprite::AnimatedSprite(float frameTime, bool paused, bool looped)
    : m_frameTime(frameTime), m_isPaused(paused), m_isLooped(looped)
{
}

void AnimatedSprite::update(float deltaTime)
{
    if (m_isPaused || !m_animation)
    {
        return;
    }

    m_currentTime += deltaTime;

    if (m_currentTime >= m_frameTime)
    {
        // Reset time, but keep the remainder so frame pacing stays exact.
        m_currentTime = std::fmod(m_currentTime, m_frameTime);

        if (m_currentFrame + 1 < m_animation->getSize())
        {
            m_currentFrame++;
        }
        else
        {
            // The animation has ended: restart, pausing if it does not loop.
            m_currentFrame = 0;

            if (!m_isLooped)
            {
                m_isPaused = true;
            }
        }

        setFrame(m_currentFrame, false);
    }
}

void AnimatedSprite::setAnimation(const Animation& animation)
{
    m_animation = &animation;

    if (const Texture* sheet = animation.getSpriteSheet())
    {
        setTexture(*sheet);
    }

    m_currentFrame = 0;
    setFrame(m_currentFrame);
}

const Animation* AnimatedSprite::getAnimation() const
{
    return m_animation;
}

void AnimatedSprite::play()
{
    m_isPaused = false;
}

void AnimatedSprite::play(const Animation& animation)
{
    if (getAnimation() != &animation)
    {
        setAnimation(animation);
    }
    play();
}

void AnimatedSprite::pause()
{
    m_isPaused = true;
}

void AnimatedSprite::stop()
{
    m_isPaused = true;
    m_currentFrame = 0;
    setFrame(m_currentFrame);
}

void AnimatedSprite::setFrameTime(float time)
{
    m_frameTime = time;
}

float AnimatedSprite::getFrameTime() const
{
    return m_frameTime;
}

void AnimatedSprite::setLooped(bool looped)
{
    m_isLooped = looped;
}

bool AnimatedSprite::isLooped() const
{
    return m_isLooped;
}

bool AnimatedSprite::isPlaying() const
{
    return !m_isPaused;
}

void AnimatedSprite::setFrame(std::size_t newFrame, bool resetTime)
{
    if (m_animation && newFrame < m_animation->getSize())
    {
        m_currentFrame = newFrame;
        setTextureRect(m_animation->getFrame(newFrame));
    }

    if (resetTime)
    {
        m_currentTime = 0.f;
    }
}

std::size_t AnimatedSprite::getCurrentFrame() const
{
    return m_currentFrame;
}

} // namespace engine
