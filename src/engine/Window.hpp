#pragma once

#include "engine/Color.hpp"
#include "engine/Drawable.hpp"
#include "engine/Vec2.hpp"

#include <string>

struct SDL_Renderer;
struct SDL_Window;

namespace engine
{

// Owns the OS window and its renderer; the single render target.
class Window
{
  public:
    Window(const std::string& title, int width, int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool isOpen() const;
    void close();

    void clear(const Color& color = Color::Black);
    void draw(const Drawable& drawable);
    void drawLine(const Vec2f& from, const Vec2f& to, const Color& color = Color::White);
    // Renders SDL's built-in 8x8 monospace debug font, scaled.
    void drawText(const Vec2f& pos, const std::string& text, const Color& color = Color::White,
                  float scale = 1.f);
    void display();

    // How far the current render frame sits between the last two physics
    // ticks (0..1); drawables use it to interpolate their position.
    void setFrameAlpha(float alpha);
    float getFrameAlpha() const;

    SDL_Renderer* getRenderer() const;

  private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    bool m_open = false;
    float m_frame_alpha = 1.f;
};

} // namespace engine
