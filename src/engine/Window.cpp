#include "engine/Window.hpp"

#include <SDL3/SDL.h>

namespace engine
{

Window::Window(const std::string& title, int width, int height) : m_width(width), m_height(height)
{
    if (!SDL_CreateWindowAndRenderer(title.c_str(), width, height, 0, &m_window, &m_renderer))
    {
        SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        return;
    }

    // Cap the frame rate to the display's refresh instead of busy-looping.
    SDL_SetRenderVSync(m_renderer, 1);

    // The game always renders at its fixed size; SDL maps it onto
    // whatever the window really measures.
    setScaleMode(m_scale_mode);

    m_open = true;
}

Window::~Window()
{
    if (m_renderer)
    {
        SDL_DestroyRenderer(m_renderer);
    }
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
    }
}

bool Window::isOpen() const
{
    return m_open;
}

void Window::close()
{
    m_open = false;
}

void Window::setFullscreen(bool enabled)
{
    if (SDL_SetWindowFullscreen(m_window, enabled))
    {
        m_fullscreen = enabled;
    }
}

bool Window::isFullscreen() const
{
    return m_fullscreen;
}

void Window::setScaleMode(ScaleMode mode)
{
    const SDL_RendererLogicalPresentation presentation = mode == ScaleMode::Letterbox
                                                             ? SDL_LOGICAL_PRESENTATION_LETTERBOX
                                                             : SDL_LOGICAL_PRESENTATION_STRETCH;

    if (SDL_SetRenderLogicalPresentation(m_renderer, m_width, m_height, presentation))
    {
        m_scale_mode = mode;
    }
}

Window::ScaleMode Window::getScaleMode() const
{
    return m_scale_mode;
}

Vec2f Window::getMousePosition() const
{
    float window_x = 0.f;
    float window_y = 0.f;
    SDL_GetMouseState(&window_x, &window_y);

    float x = 0.f;
    float y = 0.f;
    SDL_RenderCoordinatesFromWindow(m_renderer, window_x, window_y, &x, &y);

    return {x, y};
}

void Window::clear(const Color& color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(m_renderer);
}

void Window::draw(const Drawable& drawable)
{
    drawable.draw(*this);
}

void Window::drawLine(const Vec2f& from, const Vec2f& to, const Color& color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLine(m_renderer, from.x, from.y, to.x, to.y);
}

void Window::drawText(const Vec2f& pos, const std::string& text, const Color& color, float scale)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

    // The debug font has a fixed 8px glyph size; scale the whole renderer
    // around the call to enlarge it.
    SDL_SetRenderScale(m_renderer, scale, scale);
    SDL_RenderDebugText(m_renderer, pos.x / scale, pos.y / scale, text.c_str());
    SDL_SetRenderScale(m_renderer, 1.f, 1.f);
}

void Window::display()
{
    SDL_RenderPresent(m_renderer);
}

void Window::setFrameAlpha(float alpha)
{
    m_frame_alpha = alpha;
}

float Window::getFrameAlpha() const
{
    return m_frame_alpha;
}

SDL_Renderer* Window::getRenderer() const
{
    return m_renderer;
}

} // namespace engine
