#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <set>
#include <vector>

#include "GLFW/glfw3.h"
#include "box2d/box2d.h"

class ContactListener : public b2ContactListener {
public:
    void registerCallbackFn(std::function<void(b2Body*, b2Body*, bool)> callback) { m_callback = callback; }

private:
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;

    std::function<void(b2Body*, b2Body*, bool)> m_callback;
};

class Game {
public:
    Game() = default;
    ~Game() = default;

    bool init(std::shared_ptr<b2World>);
    void imGuiUpdate();
    void update();
    void keyCallback(int key, int scancode, int action, int mods);

private:
    void createTrack();
    void createTriangle();
    void createGoal();
    void resetTriangle();

    void collisionCallback(b2Body* bodyA, b2Body* bodyB, bool hasContact);
    bool isGoal(b2Body* bodyA, b2Body* bodyB) const;
    void restartLapTimer();

    std::shared_ptr<b2World> m_world;
    std::set<b2Body*> m_toDelete;

    ContactListener m_contactListener;
    b2Body* m_triangle;
    b2Body* m_goal;

    std::chrono::time_point<std::chrono::steady_clock> m_lapTimer;
    bool m_lapTimerStarted { false };
    std::vector<int> m_lapTimes;

    bool m_keyPressed[GLFW_KEY_LAST] = { false };
};
