#include "engine/Window.hpp"

#include <SDL3/SDL.h>

namespace engine
{

Window::Window(const std::string& title, int width, int height)
{
    if (!SDL_CreateWindowAndRenderer(title.c_str(), width, height, 0, &m_window, &m_renderer))
    {
        SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        return;
    }

    // Cap the frame rate to the display's refresh instead of busy-looping.
    SDL_SetRenderVSync(m_renderer, 1);

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

void Window::display()
{
    SDL_RenderPresent(m_renderer);
}

SDL_Renderer* Window::getRenderer() const
{
    return m_renderer;
}

} // namespace engine
