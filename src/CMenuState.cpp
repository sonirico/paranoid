#include "CMenuState.hpp"

#include "CGameContainer.hpp"
#include "engine/Text.hpp"

#include <SDL3/SDL.h>

using namespace game::game_states;

CMenuState::CMenuState(CGameContainer* gc)
{
    this->gc = gc;
    this->rh = gc->rh;
}

void CMenuState::init()
{
    this->gc->play_music("media/music/mainmenu.ogg", true);

    this->menu = std::make_unique<CMenu>(
        this->gc, std::vector<std::string>{"PLAY", "OPTIONS", "EDITOR", "QUIT"}, game::WIDTH / 2.f,
        300.f);

    this->options_menu = std::make_unique<CMenu>(
        this->gc, std::vector<std::string>{"", "", "", "", "BACK"}, game::WIDTH / 2.f, 280.f);

    this->refresh_options();
}

void CMenuState::events() {}

int CMenuState::update(const float)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);
    const bool esc = keys[SDL_SCANCODE_ESCAPE];

    if (esc && !this->esc_was_down && this->in_options)
    {
        this->in_options = false;
    }
    this->esc_was_down = esc;

    if (!this->in_options)
    {
        switch (this->menu->update())
        {
        case 0:
            return PLAY;
        case 1:
            this->in_options = true;
            break;
        case 2:
            return MAP;
        case 3:
            this->gc->window->close();
            break;
        }

        return NULLSTATE;
    }

    // Each activation steps the volume by 10, wrapping past 100 to 0.
    const auto next_volume = [](float volume) { return volume >= 100.f ? 0.f : volume + 10.f; };

    const int activated = this->options_menu->update();

    // Left/Right adjusts the selected entry in place: toggles flip and
    // volumes step by 10 without wrapping.
    const int step = this->options_menu->get_horizontal();

    if (step != 0)
    {
        switch (this->options_menu->get_selected())
        {
        case 0:
            this->gc->window->setScaleMode(this->gc->window->getScaleMode() ==
                                                   engine::Window::ScaleMode::Letterbox
                                               ? engine::Window::ScaleMode::Stretch
                                               : engine::Window::ScaleMode::Letterbox);
            break;
        case 1:
            this->gc->window->setFullscreen(!this->gc->window->isFullscreen());
            break;
        case 2:
            this->gc->set_music_volume(this->gc->get_music_volume() + 10.f * step);
            break;
        case 3:
            this->gc->set_fx_volume(this->gc->get_fx_volume() + 10.f * step);
            break;
        }

        this->gc->play_fx(game::game_fx::SELECT);
    }

    switch (activated)
    {
    case 0:
        this->gc->window->setScaleMode(this->gc->window->getScaleMode() ==
                                               engine::Window::ScaleMode::Letterbox
                                           ? engine::Window::ScaleMode::Stretch
                                           : engine::Window::ScaleMode::Letterbox);
        break;
    case 1:
        this->gc->window->setFullscreen(!this->gc->window->isFullscreen());
        break;
    case 2:
        this->gc->set_music_volume(next_volume(this->gc->get_music_volume()));
        break;
    case 3:
        this->gc->set_fx_volume(next_volume(this->gc->get_fx_volume()));
        break;
    case 4:
        this->in_options = false;
        break;
    }

    // F11 can also flip the fullscreen under us: mirror reality every tick.
    this->refresh_options();

    return NULLSTATE;
}

void CMenuState::render()
{
    engine::Text title;
    title.setFont(this->gc->font);
    title.setString(game::TITLE);
    title.setScale({8.f, 8.f});

    const engine::FloatRect bounds = title.getGlobalBounds();
    title.setPosition((game::WIDTH - bounds.width) / 2, 120.f);

    this->gc->window->draw(title);

    if (this->in_options)
    {
        this->options_menu->render();
    }
    else
    {
        this->menu->render();
    }
}

void CMenuState::refresh_options()
{
    const bool letterbox = this->gc->window->getScaleMode() == engine::Window::ScaleMode::Letterbox;

    this->options_menu->set_entry(0,
                                  std::string("SCALE: ") + (letterbox ? "LETTERBOX" : "STRETCH"));
    this->options_menu->set_entry(1, std::string("FULLSCREEN: ") +
                                         (this->gc->window->isFullscreen() ? "ON" : "OFF"));
    this->options_menu->set_entry(
        2, "MUSIC VOL: " + std::to_string(static_cast<int>(this->gc->get_music_volume())));
    this->options_menu->set_entry(
        3, "FX VOL: " + std::to_string(static_cast<int>(this->gc->get_fx_volume())));
}

void CMenuState::clear() {}
