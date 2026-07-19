#include "CLaser.hpp"

#include "engine/Window.hpp"

CLaser::CLaser(CState* st)
{
    this->state = st;

    this->init();
}

void CLaser::init()
{
    this->velocity = engine::Vec2f{0, -600};

    // The bolt is pure primitives; its size doubles as the hitbox.
    this->bounds = {4.f, 18.f};
}

void CLaser::update(const float dt)
{
    this->move(this->velocity * dt);

    if (this->getPosition().y + this->bounds.y < 0)
    {
        this->removable = true;
    }
}

void CLaser::reset() {}

void CLaser::draw(engine::Window& target) const
{
    // Interpolated like sprite entities, so the fast bolt stays smooth.
    engine::Vec2f pos = this->getPosition();

    if (this->has_prev_position)
    {
        pos = this->prev_position + (pos - this->prev_position) * target.getFrameAlpha();
    }

    // A red casing around a white-hot core...
    target.drawRect(pos - engine::Vec2f{1.f, 1.f}, {this->bounds.x + 2.f, this->bounds.y + 2.f},
                    {255, 60, 30, 110});
    target.drawRect(pos, this->bounds, {255, 120, 50, 255});
    target.drawRect(pos + engine::Vec2f{1.f, 2.f}, {this->bounds.x - 2.f, this->bounds.y - 6.f},
                    {255, 235, 170, 255});

    // ...trailing a short wake that fades behind the climb.
    for (int i = 1; i <= 3; ++i)
    {
        engine::Color wake{255, 90, 40, static_cast<std::uint8_t>(120 - i * 35)};

        target.drawRect(pos + engine::Vec2f{0.f, this->bounds.y + i * 5.f}, {this->bounds.x, 4.f},
                        wake);
    }
}

bool CLaser::collision_laser_brick(CBrick* b)
{
    float x1, y1, w1, h1;
    float x2, y2, w2, h2;

    x1 = this->getPosition().x;
    y1 = this->getPosition().y;
    w1 = this->bounds.x;
    h1 = this->bounds.y;

    x2 = b->getPosition().x;
    y2 = b->getPosition().y;
    w2 = b->get_bounds().width * 2;
    h2 = b->get_bounds().height * 2;

    if ((x1 + w1) < x2)
        return false;
    if ((y1 + h1) < y2)
        return false;
    if ((x2 + w2) < x1)
        return false;
    if ((y2 + h2) < y1)
        return false;

    this->removable = true;

    return true;
}

bool CLaser::is_removable() const
{
    return this->removable;
}
