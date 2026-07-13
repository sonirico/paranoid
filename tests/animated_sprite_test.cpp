#include "engine/AnimatedSprite.hpp"

#include <gtest/gtest.h>

namespace
{
engine::Animation newTestAnimation(std::size_t frames)
{
    engine::Animation animation;

    for (std::size_t i = 0; i < frames; ++i)
    {
        animation.addFrame({static_cast<int>(i) * 8, 0, 8, 8});
    }

    return animation;
}
} // namespace

TEST(AnimatedSprite, StartsOnFirstFrame)
{
    const engine::Animation animation = newTestAnimation(3);
    engine::AnimatedSprite sprite(0.1f);

    sprite.play(animation);

    EXPECT_EQ(sprite.getCurrentFrame(), 0u);
    EXPECT_TRUE(sprite.isPlaying());
}

TEST(AnimatedSprite, AdvancesFrameWhenFrameTimeElapses)
{
    const engine::Animation animation = newTestAnimation(3);
    engine::AnimatedSprite sprite(0.1f);
    sprite.play(animation);

    sprite.update(0.1f);

    EXPECT_EQ(sprite.getCurrentFrame(), 1u);
}

TEST(AnimatedSprite, KeepsTimeRemainderAcrossFrames)
{
    const engine::Animation animation = newTestAnimation(3);
    engine::AnimatedSprite sprite(0.1f);
    sprite.play(animation);

    // 0.15s = one full frame plus 0.05s carried over; the next
    // 0.05s must complete the second frame.
    sprite.update(0.15f);
    sprite.update(0.05f);

    EXPECT_EQ(sprite.getCurrentFrame(), 2u);
}

TEST(AnimatedSprite, WrapsToFirstFrameWhenLooped)
{
    const engine::Animation animation = newTestAnimation(2);
    engine::AnimatedSprite sprite(0.1f, false, true);
    sprite.play(animation);

    sprite.update(0.1f);
    sprite.update(0.1f);

    EXPECT_EQ(sprite.getCurrentFrame(), 0u);
    EXPECT_TRUE(sprite.isPlaying());
}

TEST(AnimatedSprite, PausesAtEndWhenNotLooped)
{
    const engine::Animation animation = newTestAnimation(2);
    engine::AnimatedSprite sprite(0.1f, false, false);
    sprite.play(animation);

    sprite.update(0.1f);
    sprite.update(0.1f);

    EXPECT_EQ(sprite.getCurrentFrame(), 0u);
    EXPECT_FALSE(sprite.isPlaying());
}

TEST(AnimatedSprite, StopResetsToFirstFrameAndPauses)
{
    const engine::Animation animation = newTestAnimation(3);
    engine::AnimatedSprite sprite(0.1f);
    sprite.play(animation);
    sprite.update(0.1f);

    sprite.stop();

    EXPECT_EQ(sprite.getCurrentFrame(), 0u);
    EXPECT_FALSE(sprite.isPlaying());
}

TEST(AnimatedSprite, UpdateWithoutAnimationIsHarmless)
{
    engine::AnimatedSprite sprite(0.1f);

    sprite.update(1.f);

    EXPECT_EQ(sprite.getCurrentFrame(), 0u);
}

TEST(AnimatedSprite, GlobalBoundsFollowTransform)
{
    const engine::Animation animation = newTestAnimation(1);
    engine::AnimatedSprite sprite(0.1f);
    sprite.play(animation);
    sprite.setPosition({100.f, 50.f});
    sprite.setScale({2.f, 2.f});

    const engine::FloatRect bounds = sprite.getGlobalBounds();

    EXPECT_FLOAT_EQ(bounds.left, 100.f);
    EXPECT_FLOAT_EQ(bounds.top, 50.f);
    EXPECT_FLOAT_EQ(bounds.width, 16.f);
    EXPECT_FLOAT_EQ(bounds.height, 16.f);
}
