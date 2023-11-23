#pragma once

#include <memory>
#include <set>

#include "box2d/box2d.h"

class Game {
public:
    Game() = default;
    ~Game() = default;

    bool init(std::shared_ptr<b2World>);
    void update();
    void keyCallback(int key, int scancode, int action, int mods);

private:
    std::shared_ptr<b2World> m_world;
    std::set<b2Body*> m_toDelete;
};
