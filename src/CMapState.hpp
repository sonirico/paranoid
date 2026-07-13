#pragma once

#include "CBall.hpp"
#include "CBrick.hpp"
#include "CPaddle.hpp"
#include "CState.hpp"
#include "assets.h"

#include <list>
#include <memory>

class CGameContainer;
struct SSelector;

// The stage editor: move a selector over the grid, place brick types,
// and dump the resulting map to stdout.
class CMapState : public CState
{
  public:
    explicit CMapState(CGameContainer* gc);
    ~CMapState() override;

    void init() override;
    void events() override;
    int update(const float dt) override;
    void render() override;
    void clear() override;

  private:
    void update_balls(const float dt);
    void update_bricks(const float dt);
    void update_paddle(const float dt);

    void render_balls();
    void render_bricks();
    void render_paddle();

  private:
    std::unique_ptr<CPaddle> paddle;

    std::list<std::unique_ptr<CBall>> balls;
    std::list<std::unique_ptr<CBrick>> bricks;

    std::unique_ptr<SSelector> selector;
};
