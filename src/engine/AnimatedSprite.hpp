#pragma once

#include "engine/Animation.hpp"
#include "engine/Sprite.hpp"

#include <cstddef>

namespace engine
{

// Sprite that steps through an Animation's frames on update().
// Port of the classic SFML community AnimatedSprite, with times
// expressed in float seconds.
class AnimatedSprite : public Sprite
{
  public:
    explicit AnimatedSprite(float frameTime = 0.2f, bool paused = false, bool looped = true);

    void update(float deltaTime);

    void setAnimation(const Animation& animation);
    const Animation* getAnimation() const;

    void play();
    void play(const Animation& animation);
    void pause();
    void stop();

    void setFrameTime(float time);
    float getFrameTime() const;

    void setLooped(bool looped);
    bool isLooped() const;
    bool isPlaying() const;

    void setFrame(std::size_t newFrame, bool resetTime = true);
    std::size_t getCurrentFrame() const;

  private:
    const Animation* m_animation = nullptr;
    float m_frameTime;
    float m_currentTime = 0.f;
    std::size_t m_currentFrame = 0;
    bool m_isPaused;
    bool m_isLooped;
};

} // namespace engine
