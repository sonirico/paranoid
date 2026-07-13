#include "engine/Sprite.hpp"

#include <gtest/gtest.h>

TEST(Sprite, LocalBoundsMatchTextureRect)
{
    engine::Sprite sprite;

    sprite.setTextureRect({16, 32, 8, 4});

    const engine::FloatRect bounds = sprite.getLocalBounds();

    EXPECT_FLOAT_EQ(bounds.left, 0.f);
    EXPECT_FLOAT_EQ(bounds.top, 0.f);
    EXPECT_FLOAT_EQ(bounds.width, 8.f);
    EXPECT_FLOAT_EQ(bounds.height, 4.f);
}

TEST(Sprite, GlobalBoundsApplyPositionScaleAndOrigin)
{
    engine::Sprite sprite;
    sprite.setTextureRect({0, 0, 10, 10});
    sprite.setPosition({50.f, 60.f});
    sprite.setScale({2.f, 3.f});
    sprite.setOrigin({5.f, 5.f});

    const engine::FloatRect bounds = sprite.getGlobalBounds();

    EXPECT_FLOAT_EQ(bounds.left, 40.f);
    EXPECT_FLOAT_EQ(bounds.top, 45.f);
    EXPECT_FLOAT_EQ(bounds.width, 20.f);
    EXPECT_FLOAT_EQ(bounds.height, 30.f);
}

TEST(Sprite, MoveShiftsPosition)
{
    engine::Sprite sprite;
    sprite.setPosition({10.f, 10.f});

    sprite.move({5.f, -2.f});

    EXPECT_EQ(sprite.getPosition(), (engine::Vec2f{15.f, 8.f}));
}
