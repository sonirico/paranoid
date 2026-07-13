#include "CResourceHolder.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <gtest/gtest.h>
#include <stdexcept>

class ResourceHolderTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO)) << SDL_GetError();

        window = std::make_unique<engine::Window>("test", 320, 240);
        ASSERT_TRUE(window->isOpen());

        holder = std::make_unique<CResourceHolder>(window->getRenderer());
    }

    void TearDown() override
    {
        holder.reset();
        window.reset();
        SDL_Quit();
    }

    std::unique_ptr<engine::Window> window;
    std::unique_ptr<CResourceHolder> holder;
};

TEST_F(ResourceHolderTest, LoadsAndReturnsTexture)
{
    holder->load(game::game_textures::MAIN, TEST_MEDIA_DIR "/textures/base.png");

    const engine::Texture& texture = holder->get(game::game_textures::MAIN);

    EXPECT_GT(texture.getSize().x, 0);
}

TEST_F(ResourceHolderTest, ThrowsWhenTextureFileIsMissing)
{
    EXPECT_THROW(holder->load(game::game_textures::MAIN, TEST_MEDIA_DIR "/nope.png"),
                 std::runtime_error);
}

TEST_F(ResourceHolderTest, LoadsAndReturnsSoundBuffer)
{
    holder->load(game::game_fx::REBOTE1, TEST_MEDIA_DIR "/fx/rebote1.wav");

    const engine::SoundBuffer& buffer = holder->get(game::game_fx::REBOTE1);

    EXPECT_GT(buffer.getSampleCount(), 0u);
}

TEST_F(ResourceHolderTest, ThrowsWhenSoundFileIsMissing)
{
    EXPECT_THROW(holder->load(game::game_fx::REBOTE1, TEST_MEDIA_DIR "/fx/nope.wav"),
                 std::runtime_error);
}
