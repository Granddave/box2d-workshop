#pragma once

#include <memory>
#include <set>

#include "GLFW/glfw3.h"
#include "box2d/box2d.h"

class Game {
public:
    Game() = default;
    ~Game() = default;

    bool init(std::shared_ptr<b2World>);
    void update();
    void keyCallback(int key, int scancode, int action, int mods);

private:
    void createTrack();
    void createTriangle();
    void createGoal();
    void resetTriangle();

    std::shared_ptr<b2World> m_world;
    std::set<b2Body*> m_toDelete;

    b2Body* m_triangle;
    b2Body* m_goal;

    bool m_keyPressed[GLFW_KEY_LAST] = { false };
};
