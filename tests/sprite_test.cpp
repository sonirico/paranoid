#include "engine/Sprite.hpp"
#include "engine/Texture.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

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

// Renders through SDL's dummy driver to catch sampling artifacts a
// pure-math test cannot see.
class SpriteRenderTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO)) << SDL_GetError();

        window = std::make_unique<engine::Window>("test", 320, 240);
        ASSERT_TRUE(window->isOpen()) << SDL_GetError();

        ASSERT_TRUE(
            texture.loadFromFile(window->getRenderer(), TEST_MEDIA_DIR "/textures/base.png"));
    }

    void TearDown() override
    {
        window.reset();
        SDL_Quit();
    }

    std::unique_ptr<engine::Window> window;
    engine::Texture texture;
};

TEST_F(SpriteRenderTest, FractionalPositionsSnapToTheNearestWholePixel)
{
    // A fractional destination rect makes GPU renderers sample half a
    // texel outside the source rect, bleeding the atlas neighbor into
    // the sprite's edge. Sprites must therefore render snapped to the
    // nearest whole pixel: at x = 100.75 the edge belongs to 101.
    engine::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect({128, 80, 32, 8}); // A paddle frame.
    sprite.setScale({2.f, 2.f});
    sprite.setPosition({100.75f, 100.f});

    window->clear({0, 0, 255, 255});
    window->draw(sprite);

    SDL_Surface* shot = SDL_RenderReadPixels(window->getRenderer(), nullptr);
    ASSERT_NE(shot, nullptr) << SDL_GetError();

    std::uint8_t r = 0, g = 0, b = 0, a = 0;

    // Left of the snapped edge: still background.
    ASSERT_TRUE(SDL_ReadSurfacePixel(shot, 100, 106, &r, &g, &b, &a));
    EXPECT_EQ(r, 0);
    EXPECT_EQ(b, 255);

    // On the snapped edge: the frame's own leftmost texel.
    ASSERT_TRUE(SDL_ReadSurfacePixel(shot, 101, 106, &r, &g, &b, &a));
    EXPECT_EQ(r, 247);
    EXPECT_EQ(g, 247);

    SDL_DestroySurface(shot);
}
