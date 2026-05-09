#include "physics_engine.h"
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>

static const char* COLORS[] = {
    "\033[31m", "\033[32m", "\033[33m", "\033[34m",
    "\033[35m", "\033[36m", "\033[37m", "\033[91m",
    "\033[92m", "\033[93m", "\033[94m", "\033[95m", "\033[96m"
};

static std::string randomColor() {
    return COLORS[rand() % 13];
}

PhysicsEngine::PhysicsEngine(int w, int h)
    : world(b2Vec2(0.0f, 10.0f)), width(w), height(h), scale(2.0f) {
    addGround();
    addWall(-0.5f / scale, height / (2.0f * scale), 0.5f, height / scale);
    addWall(width / scale + 0.5f / scale, height / (2.0f * scale), 0.5f, height / scale);
}

void PhysicsEngine::addGround() {
    b2BodyDef groundDef;
    groundDef.position.Set(width / (2.0f * scale), height / scale + 0.5f);
    b2Body* ground = world.CreateBody(&groundDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(width / (2.0f * scale), 0.5f);
    ground->CreateFixture(&groundBox, 0.0f);
}

void PhysicsEngine::addWall(float x, float y, float w, float h) {
    b2BodyDef wallDef;
    wallDef.position.Set(x, y);
    b2Body* wall = world.CreateBody(&wallDef);

    b2PolygonShape wallBox;
    wallBox.SetAsBox(w, h);
    wall->CreateFixture(&wallBox, 0.0f);
}

void PhysicsEngine::addBox(float x, float y, float w, float h) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    bodyDef.angle = (rand() % 100) / 100.0f * 3.14f;
    b2Body* body = world.CreateBody(&bodyDef);

    b2PolygonShape box;
    box.SetAsBox(w, h);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.6f;
    body->CreateFixture(&fixtureDef);

    bodies.push_back({body, '#', std::max(w, h) * scale, false, randomColor()});
}

void PhysicsEngine::addCircle(float x, float y, float r) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    b2Body* body = world.CreateBody(&bodyDef);

    b2CircleShape circle;
    circle.m_radius = r;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.7f;
    body->CreateFixture(&fixtureDef);

    bodies.push_back({body, 'O', r * scale, true, randomColor()});
}

void PhysicsEngine::step(float timeStep) {
    const int velocityIterations = 6;
    const int positionIterations = 2;
    world.Step(timeStep, velocityIterations, positionIterations);
}

void PhysicsEngine::reset() {
    for (auto& pb : bodies) {
        world.DestroyBody(pb.body);
    }
    bodies.clear();
}
