#include <SDL3/SDL.h>

#include <gtest/gtest.h>

TEST(BuildSmoke, LinksAgainstSdl3)
{
    const int version = SDL_GetVersion();

    EXPECT_GE(SDL_VERSIONNUM_MAJOR(version), 3);
}

TEST(BuildSmoke, InitializesHeadlessSdl)
{
    ASSERT_TRUE(SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy"));
    ASSERT_TRUE(SDL_Init(SDL_INIT_VIDEO)) << SDL_GetError();

    SDL_Quit();
}
