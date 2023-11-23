#include "game.hpp"

#include <iostream>

bool Game::init(std::shared_ptr<b2World> world) {
    m_world = world;

    // Reset the world
    m_world->SetGravity({ 0.f, 0.f });

    // TODO
    // CollisionListener cl;
    // g_world->SetContactListener(&cl);

    // Some starter objects are created here, such as the ground
    b2Body* ground;
    b2EdgeShape ground_shape;
    ground_shape.SetTwoSided(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
    b2BodyDef ground_bd;
    ground = m_world->CreateBody(&ground_bd);
    ground->CreateFixture(&ground_shape, 0.0f);

    b2Body* box;
    b2PolygonShape box_shape;
    box_shape.SetAsBox(1.0f, 1.0f);

    b2FixtureDef box_fd;
    box_fd.shape = &box_shape;
    box_fd.density = 200.0f;
    box_fd.friction = 0.1f;
    b2BodyDef box_bd;
    box_bd.type = b2_dynamicBody;
    box_bd.position.Set(-10.0f, 11.25f);
    box = m_world->CreateBody(&box_bd);
    box->CreateFixture(&box_fd);
    box->SetAngularVelocity(10.0f);
    box->SetLinearVelocity({ 5.f, 0.f });

    {
        b2PolygonShape shape;
        shape.SetAsBox(0.1f, 1.0f);

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 2.0f;
        fd.friction = 0.1f;

        for (int i = 0; i < 10; ++i) {
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(-4.5f + 1.0f * i, 1.0f);
            b2Body* body = m_world->CreateBody(&bd);
            body->CreateFixture(&fd);
        }
    }

    return true;
}

void Game::update() {
    for (auto* body : m_toDelete) {
        m_world->DestroyBody(body);
    }
    m_toDelete.clear();
}

void Game::keyCallback(int key, int scancode, int action, int mods) {
    std::cerr << "keyCallback: " << key << ", " << scancode << ", " << action << ", " << mods << std::endl;
}
