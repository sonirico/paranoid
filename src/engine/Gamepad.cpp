#include "engine/Gamepad.hpp"

#include <SDL3/SDL.h>

namespace engine
{

namespace
{

SDL_GamepadButton toSdlButton(Gamepad::Button button)
{
    switch (button)
    {
    case Gamepad::Button::South:
        return SDL_GAMEPAD_BUTTON_SOUTH;
    case Gamepad::Button::Start:
        return SDL_GAMEPAD_BUTTON_START;
    case Gamepad::Button::DpadUp:
        return SDL_GAMEPAD_BUTTON_DPAD_UP;
    case Gamepad::Button::DpadDown:
        return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
    case Gamepad::Button::DpadLeft:
        return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
    case Gamepad::Button::DpadRight:
        return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
    }

    return SDL_GAMEPAD_BUTTON_INVALID;
}

} // namespace

Gamepad::~Gamepad()
{
    if (m_gamepad != nullptr)
    {
        SDL_CloseGamepad(m_gamepad);
    }
}

void Gamepad::update()
{
    if (m_gamepad != nullptr)
    {
        if (!SDL_GamepadConnected(m_gamepad))
        {
            SDL_CloseGamepad(m_gamepad);
            m_gamepad = nullptr;
        }

        return;
    }

    int count = 0;

    if (SDL_JoystickID* ids = SDL_GetGamepads(&count))
    {
        if (count > 0)
        {
            m_gamepad = SDL_OpenGamepad(ids[0]);
        }

        SDL_free(ids);
    }
}

bool Gamepad::isConnected() const
{
    return m_gamepad != nullptr;
}

bool Gamepad::isButtonDown(Button button) const
{
    if (m_gamepad == nullptr)
    {
        return false;
    }

    return SDL_GetGamepadButton(m_gamepad, toSdlButton(button));
}

float Gamepad::getLeftStickX() const
{
    if (m_gamepad == nullptr)
    {
        return 0.f;
    }

    const float value = SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTX) / 32767.f;

    return value > DEAD_ZONE || value < -DEAD_ZONE ? value : 0.f;
}

} // namespace engine
