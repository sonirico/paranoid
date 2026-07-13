#pragma once

class CGameContainer;
class CResourceHolder;

// Interface every game screen (play, map editor, ...) implements.
// update() returns the id of the state to switch to, or NULLSTATE.
class CState
{
  public:
    virtual ~CState() = default;

    virtual void init() = 0;
    virtual void events() = 0;
    virtual int update(const float dt) = 0;
    virtual void render() = 0;
    virtual void clear() = 0;

  public:
    CGameContainer* gc = nullptr;
    CResourceHolder* rh = nullptr;
};
