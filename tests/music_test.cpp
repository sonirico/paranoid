#include "engine/AudioDevice.hpp"
#include "engine/Music.hpp"

#include <SDL3/SDL.h>

#include <gtest/gtest.h>

// Runs against SDL's dummy audio driver, so it is headless and silent.
class MusicTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy"));
        ASSERT_TRUE(SDL_Init(SDL_INIT_AUDIO)) << SDL_GetError();
    }

    void TearDown() override
    {
        SDL_Quit();
    }
};

TEST_F(MusicTest, DecodesOggFromDisk)
{
    engine::AudioDevice device;
    engine::Music music(device);

    const bool loaded = music.loadFromFile(TEST_MEDIA_DIR "/music/stage.ogg");

    ASSERT_TRUE(loaded);
    EXPECT_GT(music.getSampleCount(), 0u);
}

TEST_F(MusicTest, FailsToLoadMissingFile)
{
    engine::AudioDevice device;
    engine::Music music(device);

    EXPECT_FALSE(music.loadFromFile(TEST_MEDIA_DIR "/music/does_not_exist.ogg"));
}

TEST_F(MusicTest, PlaysAndLoopsWithoutCrashing)
{
    engine::AudioDevice device;
    ASSERT_TRUE(device.isOpen());

    engine::Music music(device);
    ASSERT_TRUE(music.loadFromFile(TEST_MEDIA_DIR "/music/stage.ogg"));

    music.setVolume(30.f);
    music.play();

    // The update hook keeps the stream queue topped up for looping.
    for (int i = 0; i < 10; ++i)
    {
        music.update();
    }
}
