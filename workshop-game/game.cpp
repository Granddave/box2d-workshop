#include "game.hpp"

#include <iostream>
#include <math.h>

/**
 * Game featuers:
 * - Penalty time
 * - Time per lap
 * - Lap counter
 * - Lap time
 */

void ContactListener::BeginContact(b2Contact* contact) {
    if (m_callback) {
        m_callback(contact->GetFixtureA()->GetBody(), contact->GetFixtureB()->GetBody(), true);
    }
}

void ContactListener::EndContact(b2Contact* contact) {
    if (m_callback) {
        m_callback(contact->GetFixtureA()->GetBody(), contact->GetFixtureB()->GetBody(), false);
    }
}

bool Game::init(std::shared_ptr<b2World> world) {
    m_world = world;
    m_world->SetGravity({ 0.f, 0.f });
    m_world->SetContactListener(&m_contactListener);

    m_contactListener.registerCallbackFn(std::bind(&Game::collisionCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    createTrack();
    createTriangle();
    createGoal();

    return true;
}

void Game::collisionCallback(b2Body* bodyA, b2Body* bodyB, bool hasContact) {
    if (isGoal(bodyA, bodyB)) {
        if (hasContact) {
            std::cout << "Goal!" << std::endl;
        } else {
            std::cout << "Starting timer" << std::endl;
        }
    }
}

bool Game::isGoal(b2Body* bodyA, b2Body* bodyB) const {
    return (bodyA == m_triangle && bodyB == m_goal) || (bodyA == m_goal && bodyB == m_triangle);
}

void Game::createTrack() {
    enum class Orientation {
        Horizontal,
        Vertical
    };
    auto createWall = [&](float posX, float posY, float length, Orientation orientation) {
        b2BodyDef bodyDef;
        bodyDef.position.Set(posX, posY);
        bodyDef.type = b2_staticBody;
        b2Body* body = m_world->CreateBody(&bodyDef);

        b2PolygonShape shape;
        float wallThickness = 0.1f;
        if (orientation == Orientation::Horizontal)
            shape.SetAsBox(length / 2, wallThickness);
        else if (orientation == Orientation::Vertical)
            shape.SetAsBox(wallThickness, length / 2);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);
    };

    /*
    ----------------------
    |                    |
    |    --------------  |
    |          |         |
    |          |  -------|
    |-----     |  |      |
    |          |  |  |   |
    |          |  |  |   |
    |          |     |   |
    |   --------------   |
    |                    |
    ----------------------
    */
    createWall(0, 0, 40, Orientation::Horizontal);
    createWall(0, 40, 40, Orientation::Horizontal);
    createWall(-20, 20, 40, Orientation::Vertical);
    createWall(20, 20, 40, Orientation::Vertical);

    createWall(0, 5, 20, Orientation::Horizontal);
    createWall(10, 10, 10, Orientation::Vertical);
    createWall(12.5, 20, 15, Orientation::Horizontal);
    createWall(5, 15, 10, Orientation::Vertical);
    createWall(0, 17.5, 25, Orientation::Vertical);
    createWall(2.5, 30, 25, Orientation::Horizontal);
    createWall(-15, 17.5, 10, Orientation::Horizontal);
}

void Game::createTriangle() {
    b2PolygonShape triangle_shape;
    b2Vec2 triangle_vertices[3];
    float witdh = 0.3f;
    float height = 1.0f;
    triangle_vertices[0].Set(-witdh, 0.0f);
    triangle_vertices[1].Set(0.0f, height);
    triangle_vertices[2].Set(witdh, 0.0f);
    triangle_shape.Set(triangle_vertices, 3);
    b2FixtureDef triangle_fd;
    triangle_fd.shape = &triangle_shape;
    triangle_fd.density = 200.0f;
    triangle_fd.friction = 0.1f;
    b2BodyDef triangle_bd;
    triangle_bd.type = b2_dynamicBody;
    triangle_bd.position.Set(.0f, .0f);

    m_triangle = m_world->CreateBody(&triangle_bd);
    m_triangle->CreateFixture(&triangle_fd);
    m_triangle->SetAngularDamping(1.0f);
    m_triangle->SetLinearDamping(1.0f);

    resetTriangle();
}

void Game::createGoal() {
    b2BodyDef bodyDef;
    bodyDef.position.Set(0, 2.5f);
    bodyDef.type = b2_staticBody;
    m_goal = m_world->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(0.01f, 5.0f / 2); // Very thin line

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true; // Make this a sensor
    m_goal->CreateFixture(&fixtureDef);
}

void Game::resetTriangle() {
    m_triangle->SetTransform({ 0.f, 2.5f }, M_PI / -2.0f);
    m_triangle->SetLinearVelocity({ 0.f, 0.f });
    m_triangle->SetAngularVelocity(0.f);
}

void Game::update() {
    // Torque
    constexpr float torque = 100.0f;
    float torqueToApply = 0.f;
    if (m_keyPressed[GLFW_KEY_A] || m_keyPressed[GLFW_KEY_LEFT]) {
        torqueToApply += torque;
    }
    if (m_keyPressed[GLFW_KEY_D] || m_keyPressed[GLFW_KEY_RIGHT]) {
        torqueToApply -= torque;
    }
    m_triangle->ApplyTorque(torqueToApply, true);

    // Thrust
    constexpr float thrust = 1000.0f;
    if (m_keyPressed[GLFW_KEY_W] || m_keyPressed[GLFW_KEY_UP]) {
        // Approximate bottom center in local coordinates of the triangle
        // Assuming the height is along the y-axis and width along the x-axis
        b2Vec2 bottomCenterLocal(0.0f, 0.0f);

        // Convert the bottom center to world coordinates
        b2Vec2 bottomCenterWorld = m_triangle->GetWorldPoint(bottomCenterLocal);

        m_triangle->ApplyForceToCenter(m_triangle->GetWorldVector({ 0.f, thrust }), true);
    }

    if (m_keyPressed[GLFW_KEY_R]) {
        resetTriangle();
    }
}

void Game::keyCallback(int key, int scancode, int action, int mods) {
    m_keyPressed[key] = action != GLFW_RELEASE;
}
