#pragma once

struct SDL_Gamepad;

namespace engine
{

// The first connected game controller. Call update() once per frame:
// it opens a pad when one appears and drops it when unplugged.
class Gamepad
{
  public:
    enum class Button
    {
        South,
        Start,
        DpadUp,
        DpadDown,
        DpadLeft,
        DpadRight
    };

    Gamepad() = default;
    ~Gamepad();

    Gamepad(const Gamepad&) = delete;
    Gamepad& operator=(const Gamepad&) = delete;

    void update();

    bool isConnected() const;

    bool isButtonDown(Button button) const;

    // Left stick X in -1..1 with the dead zone already applied.
    float getLeftStickX() const;

  private:
    SDL_Gamepad* m_gamepad = nullptr;

    static constexpr float DEAD_ZONE = 0.2f;
};

} // namespace engine
