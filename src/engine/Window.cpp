#include "engine/Window.hpp"

#include <SDL3/SDL.h>

#include <cmath>

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

    // Primitives with translucent colors (particles) blend over the scene.
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    // The game always renders at its fixed size; SDL maps it onto
    // whatever the window really measures.
    setScaleMode(m_scale_mode);

    m_open = true;
}

Window::~Window()
{
    destroyCrtResources();

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

void Window::setCrtFilter(bool enabled)
{
    if (enabled && m_crt_scene == nullptr && !createCrtResources())
    {
        return;
    }

    m_crt = enabled;
}

bool Window::isCrtFilter() const
{
    return m_crt;
}

bool Window::createCrtResources()
{
    m_crt_scene = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
                                    m_width, m_height);

    if (m_crt_scene == nullptr)
    {
        SDL_Log("CRT scene target failed: %s", SDL_GetError());
        return false;
    }

    // Linear sampling smooths the texels the barrel warp shifts around.
    SDL_SetTextureScaleMode(m_crt_scene, SDL_SCALEMODE_LINEAR);

    // A one-pixel-wide column with a dark row every few lines; drawing
    // it stretched over the scene gives the horizontal scanlines.
    SDL_Surface* scanlines = SDL_CreateSurface(1, m_height, SDL_PIXELFORMAT_RGBA32);

    if (scanlines == nullptr)
    {
        SDL_Log("CRT scanline surface failed: %s", SDL_GetError());
        destroyCrtResources();
        return false;
    }

    SDL_FillSurfaceRect(scanlines, nullptr, SDL_MapSurfaceRGBA(scanlines, 0, 0, 0, 0));

    for (int y = CRT_SCANLINE_PERIOD - 1; y < m_height; y += CRT_SCANLINE_PERIOD)
    {
        const SDL_Rect row{0, y, 1, 1};

        SDL_FillSurfaceRect(scanlines, &row,
                            SDL_MapSurfaceRGBA(scanlines, 0, 0, 0, CRT_SCANLINE_ALPHA));
    }

    m_crt_scanlines = SDL_CreateTextureFromSurface(m_renderer, scanlines);
    SDL_DestroySurface(scanlines);

    if (m_crt_scanlines == nullptr)
    {
        SDL_Log("CRT scanline texture failed: %s", SDL_GetError());
        destroyCrtResources();
        return false;
    }

    SDL_SetTextureBlendMode(m_crt_scanlines, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(m_crt_scanlines, SDL_SCALEMODE_NEAREST);

    for (int j = 0; j <= CRT_MESH_ROWS; ++j)
    {
        for (int i = 0; i <= CRT_MESH_COLS; ++i)
        {
            const float u = static_cast<float>(i) / CRT_MESH_COLS;
            const float v = static_cast<float>(j) / CRT_MESH_ROWS;

            // Centered coordinates in [-1, 1]; r2 is 0 at the middle of
            // the screen, 1 at the edge midpoints and 2 at the corners.
            const float nx = 2.f * u - 1.f;
            const float ny = 2.f * v - 1.f;
            const float r2 = nx * nx + ny * ny;

            // Corners are pulled in the hardest while the edge midpoints
            // stay on the border: the classic bulging tube outline.
            const float bulge = (1.f + CRT_CURVATURE) / (1.f + CRT_CURVATURE * r2);

            // The vignette darkens with the distance from the centre.
            const float brightness = 1.f - CRT_VIGNETTE * std::pow(r2 * 0.5f, 1.2f);

            SDL_Vertex vertex{};
            vertex.position = {(nx * bulge * 0.5f + 0.5f) * static_cast<float>(m_width),
                               (ny * bulge * 0.5f + 0.5f) * static_cast<float>(m_height)};
            vertex.color = {brightness, brightness, brightness, 1.f};
            vertex.tex_coord = {u, v};

            m_crt_mesh.push_back(vertex);
        }
    }

    for (int j = 0; j < CRT_MESH_ROWS; ++j)
    {
        for (int i = 0; i < CRT_MESH_COLS; ++i)
        {
            const int top_left = j * (CRT_MESH_COLS + 1) + i;
            const int bottom_left = top_left + CRT_MESH_COLS + 1;

            m_crt_indices.insert(m_crt_indices.end(), {top_left, top_left + 1, bottom_left,
                                                       top_left + 1, bottom_left + 1, bottom_left});
        }
    }

    return true;
}

void Window::destroyCrtResources()
{
    if (m_crt_scanlines)
    {
        SDL_DestroyTexture(m_crt_scanlines);
        m_crt_scanlines = nullptr;
    }
    if (m_crt_scene)
    {
        SDL_DestroyTexture(m_crt_scene);
        m_crt_scene = nullptr;
    }

    m_crt_mesh.clear();
    m_crt_indices.clear();
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
    // With the CRT filter on, the whole frame renders offscreen and
    // display() maps it onto the curved tube.
    if (m_crt)
    {
        SDL_SetRenderTarget(m_renderer, m_crt_scene);
    }

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

void Window::drawRect(const Vec2f& pos, const Vec2f& size, const Color& color)
{
    const SDL_FRect rect{pos.x, pos.y, size.x, size.y};

    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(m_renderer, &rect);
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
    if (m_crt)
    {
        // Reset any shake offset so the compositing never moves, warp
        // the scene onto the tube and lay the scanlines over the glass:
        // straight and evenly spaced, unlike the curved image below.
        SDL_SetRenderViewport(m_renderer, nullptr);

        SDL_SetRenderTarget(m_renderer, nullptr);
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_renderer);
        SDL_RenderGeometry(m_renderer, m_crt_scene, m_crt_mesh.data(),
                           static_cast<int>(m_crt_mesh.size()), m_crt_indices.data(),
                           static_cast<int>(m_crt_indices.size()));
        SDL_RenderTexture(m_renderer, m_crt_scanlines, nullptr, nullptr);
    }

    SDL_RenderPresent(m_renderer);

    // Any shake offset lasts a single frame.
    SDL_SetRenderViewport(m_renderer, nullptr);
}

void Window::setViewOffset(const Vec2f& offset)
{
    const SDL_Rect viewport{static_cast<int>(offset.x), static_cast<int>(offset.y), m_width,
                            m_height};

    SDL_SetRenderViewport(m_renderer, &viewport);
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
