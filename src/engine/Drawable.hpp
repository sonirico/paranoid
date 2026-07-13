#pragma once

namespace engine
{

class Window;

// Anything the window can render.
class Drawable
{
  public:
    virtual ~Drawable() = default;

    virtual void draw(Window& target) const = 0;
};

} // namespace engine
