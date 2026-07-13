#include "engine/Sprite.hpp"
#include "engine/Texture.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <gtest/gtest.h>

// Exercises the real texture loading path against a software
// renderer on SDL's dummy video driver, so it runs headless.
class TextureTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO)) << SDL_GetError();

        window = std::make_unique<engine::Window>("test", 320, 240);
        ASSERT_TRUE(window->isOpen()) << SDL_GetError();
    }

    void TearDown() override
    {
        window.reset();
        SDL_Quit();
    }

    std::unique_ptr<engine::Window> window;
};

TEST_F(TextureTest, LoadsPngFromDisk)
{
    engine::Texture texture;

    const bool loaded =
        texture.loadFromFile(window->getRenderer(), TEST_MEDIA_DIR "/textures/bricks.png");

    ASSERT_TRUE(loaded) << SDL_GetError();
    EXPECT_GT(texture.getSize().x, 0);
    EXPECT_GT(texture.getSize().y, 0);
}

TEST_F(TextureTest, FailsToLoadMissingFile)
{
    engine::Texture texture;

    EXPECT_FALSE(texture.loadFromFile(window->getRenderer(), TEST_MEDIA_DIR "/does_not_exist.png"));
}

TEST_F(TextureTest, DrawsSpriteWithoutCrashing)
{
    engine::Texture texture;
    ASSERT_TRUE(texture.loadFromFile(window->getRenderer(), TEST_MEDIA_DIR "/textures/bricks.png"));

    engine::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setPosition({10.f, 10.f});

    window->clear();
    window->draw(sprite);
    window->display();
}
