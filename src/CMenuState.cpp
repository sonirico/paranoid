#include "CMenuState.hpp"

#include "CGameContainer.hpp"
#include "engine/Text.hpp"

using namespace game::game_states;

CMenuState::CMenuState(CGameContainer* gc)
{
    this->gc = gc;
    this->rh = gc->rh;
}

void CMenuState::init()
{
    this->menu = std::make_unique<CMenu>(
        this->gc, std::vector<std::string>{"PLAY", "OPTIONS", "QUIT"}, game::WIDTH / 2.f, 300.f);
}

void CMenuState::events() {}

int CMenuState::update(const float dt)
{
    switch (this->menu->update())
    {
    case 0:
        return PLAY;
    case 1:
        this->coming_soon = true;
        break;
    case 2:
        this->gc->window->close();
        break;
    }

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

    this->menu->render();

    if (this->coming_soon)
    {
        engine::Text note;
        note.setFont(this->gc->font);
        note.setString("COMING SOON");
        note.setScale({2.f, 2.f});
        note.setColor(engine::Color::Yellow);

        const engine::FloatRect note_bounds = note.getGlobalBounds();
        note.setPosition((game::WIDTH - note_bounds.width) / 2, 500.f);

        this->gc->window->draw(note);
    }
}

void CMenuState::clear() {}
