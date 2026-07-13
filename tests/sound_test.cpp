#include "engine/AudioDevice.hpp"
#include "engine/Sound.hpp"
#include "engine/SoundBuffer.hpp"

#include <SDL3/SDL.h>

#include <gtest/gtest.h>

// Runs against SDL's dummy audio driver, so it is headless and silent.
class SoundTest : public ::testing::Test
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

TEST_F(SoundTest, LoadsWavFromDisk)
{
    engine::SoundBuffer buffer;

    const bool loaded = buffer.loadFromFile(TEST_MEDIA_DIR "/fx/rebote1.wav");

    ASSERT_TRUE(loaded) << SDL_GetError();
    EXPECT_GT(buffer.getSampleCount(), 0u);
}

TEST_F(SoundTest, FailsToLoadMissingFile)
{
    engine::SoundBuffer buffer;

    EXPECT_FALSE(buffer.loadFromFile(TEST_MEDIA_DIR "/fx/does_not_exist.wav"));
}

TEST_F(SoundTest, PlaysBufferThroughDevice)
{
    engine::AudioDevice device;
    ASSERT_TRUE(device.isOpen()) << SDL_GetError();

    engine::SoundBuffer buffer;
    ASSERT_TRUE(buffer.loadFromFile(TEST_MEDIA_DIR "/fx/rebote1.wav"));

    engine::Sound sound(device);
    sound.setBuffer(buffer);
    sound.setVolume(10.f);

    sound.play();
}
