#include "engine/Animation.hpp"
#include "engine/Texture.hpp"

#include <gtest/gtest.h>

TEST(Animation, StartsEmpty)
{
    const engine::Animation animation;

    EXPECT_EQ(animation.getSize(), 0u);
    EXPECT_EQ(animation.getSpriteSheet(), nullptr);
}

TEST(Animation, StoresFramesInOrder)
{
    engine::Animation animation;

    animation.addFrame({0, 0, 8, 8});
    animation.addFrame({8, 0, 8, 8});

    ASSERT_EQ(animation.getSize(), 2u);
    EXPECT_EQ(animation.getFrame(0), (engine::IntRect{0, 0, 8, 8}));
    EXPECT_EQ(animation.getFrame(1), (engine::IntRect{8, 0, 8, 8}));
}

TEST(Animation, TracksSpriteSheet)
{
    engine::Animation animation;
    const engine::Texture texture;

    animation.setSpriteSheet(texture);

    EXPECT_EQ(animation.getSpriteSheet(), &texture);
}
