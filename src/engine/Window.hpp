#pragma once

#include "engine/Color.hpp"
#include "engine/Drawable.hpp"
#include "engine/Vec2.hpp"

#include <string>
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Vertex;
struct SDL_Window;

namespace engine
{

// Owns the OS window and its renderer; the single render target.
class Window
{
  public:
    // How the fixed-size game view maps onto the window when their
    // sizes differ (fullscreen, resizes).
    enum class ScaleMode
    {
        Letterbox,
        Stretch
    };

    Window(const std::string& title, int width, int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool isOpen() const;
    void close();

    void setFullscreen(bool enabled);
    bool isFullscreen() const;

    void setScaleMode(ScaleMode mode);
    ScaleMode getScaleMode() const;

    // Old-tube look: the scene renders to a texture that display() maps
    // onto a barrel-curved mesh with scanlines and a corner vignette.
    void setCrtFilter(bool enabled);
    bool isCrtFilter() const;

    // The cursor in game coordinates, regardless of window size,
    // fullscreen or letterbox bars.
    Vec2f getMousePosition() const;

    // Shifts everything drawn afterwards (screen shake); display()
    // resets it for the next frame.
    void setViewOffset(const Vec2f& offset);

    void clear(const Color& color = Color::Black);
    void draw(const Drawable& drawable);
    void drawLine(const Vec2f& from, const Vec2f& to, const Color& color = Color::White);
    // Filled rectangle; the color's alpha blends over what's below.
    void drawRect(const Vec2f& pos, const Vec2f& size, const Color& color);
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
    // Lazily builds the scene target, the scanline overlay and the
    // warped mesh; false (and the filter stays off) when the renderer
    // cannot provide them.
    bool createCrtResources();
    void destroyCrtResources();

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    bool m_open = false;
    float m_frame_alpha = 1.f;

    int m_width = 0;
    int m_height = 0;
    bool m_fullscreen = false;
    ScaleMode m_scale_mode = ScaleMode::Letterbox;

    bool m_crt = false;

    // Offscreen target the whole scene renders into while the filter
    // is on, and the scanline texture blended over it before warping.
    SDL_Texture* m_crt_scene = nullptr;
    SDL_Texture* m_crt_scanlines = nullptr;

    // The barrel-curved grid the scene texture is mapped onto, with the
    // vignette baked into its vertex colors. Built once.
    std::vector<SDL_Vertex> m_crt_mesh;
    std::vector<int> m_crt_indices;

    static constexpr int CRT_MESH_COLS = 32;
    static constexpr int CRT_MESH_ROWS = 24;

    // How hard the tube bulges: corners are pulled in by roughly this
    // fraction while the edge midpoints stay on the border.
    static constexpr float CRT_CURVATURE = 0.045f;

    // Fraction of the brightness lost at the very corners.
    static constexpr float CRT_VIGNETTE = 0.5f;

    // One dark row this often, and how opaque it is.
    static constexpr int CRT_SCANLINE_PERIOD = 3;
    static constexpr std::uint8_t CRT_SCANLINE_ALPHA = 100;
};

} // namespace engine
