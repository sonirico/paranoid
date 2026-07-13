#include "engine/BitmapFont.hpp"
#include "engine/Text.hpp"
#include "engine/Texture.hpp"
#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <gtest/gtest.h>

TEST(BitmapFont, MapsPrintableCharactersToGridCells)
{
    engine::BitmapFont font;

    // 16 glyphs per row starting at space: 'A' is index 33.
    const engine::IntRect rect = font.glyphRect('A');

    EXPECT_EQ(rect.left, 8);
    EXPECT_EQ(rect.top, 16);
    EXPECT_EQ(rect.width, 8);
    EXPECT_EQ(rect.height, 8);
}

TEST(BitmapFont, FirstAndLastPrintableStayInSheet)
{
    engine::BitmapFont font;

    const engine::IntRect first = font.glyphRect(' ');
    const engine::IntRect last = font.glyphRect('~');

    EXPECT_EQ(first.left, 0);
    EXPECT_EQ(first.top, 0);
    EXPECT_EQ(last.left, 112);
    EXPECT_EQ(last.top, 40);
}

TEST(BitmapFont, UnprintableCharactersFallBackToQuestionMark)
{
    engine::BitmapFont font;

    EXPECT_EQ(font.glyphRect('\n'), font.glyphRect('?'));
    EXPECT_EQ(font.glyphRect('\t'), font.glyphRect('?'));
}

TEST(Text, LocalBoundsSpanOneGlyphPerCharacter)
{
    engine::BitmapFont font;
    engine::Text text;
    text.setFont(font);
    text.setString("SCORE");

    const engine::FloatRect bounds = text.getLocalBounds();

    EXPECT_FLOAT_EQ(bounds.width, 5 * 8.f);
    EXPECT_FLOAT_EQ(bounds.height, 8.f);
}

TEST(Text, EmptyStringHasNoBounds)
{
    engine::BitmapFont font;
    engine::Text text;
    text.setFont(font);

    const engine::FloatRect bounds = text.getLocalBounds();

    EXPECT_FLOAT_EQ(bounds.width, 0.f);
    EXPECT_FLOAT_EQ(bounds.height, 0.f);
}

TEST(Text, GlobalBoundsApplyPositionAndScale)
{
    engine::BitmapFont font;
    engine::Text text;
    text.setFont(font);
    text.setString("HI");
    text.setPosition({100.f, 50.f});
    text.setScale({2.f, 3.f});

    const engine::FloatRect bounds = text.getGlobalBounds();

    EXPECT_FLOAT_EQ(bounds.left, 100.f);
    EXPECT_FLOAT_EQ(bounds.top, 50.f);
    EXPECT_FLOAT_EQ(bounds.width, 2 * 8.f * 2.f);
    EXPECT_FLOAT_EQ(bounds.height, 8.f * 3.f);
}

// Exercises the real draw path against a software renderer on SDL's
// dummy video driver, so it runs headless.
class TextDrawTest : public ::testing::Test
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

TEST_F(TextDrawTest, DrawsWithTheLoadedFontSheet)
{
    engine::Texture texture;
    ASSERT_TRUE(texture.loadFromFile(window->getRenderer(), TEST_MEDIA_DIR "/textures/font.png"))
        << SDL_GetError();

    engine::BitmapFont font;
    font.setTexture(texture);

    engine::Text text;
    text.setFont(font);
    text.setString("PAUSED");
    text.setColor(engine::Color::White);

    window->clear();
    window->draw(text);
    window->display();
}
