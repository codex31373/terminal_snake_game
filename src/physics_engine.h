#pragma once

#include <vector>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>

struct PhysicsBody {
    b2Body* body;
    char symbol;
    float radius;
    bool isCircle;
};

class PhysicsEngine {
private:
    b2World world;
    std::vector<PhysicsBody> bodies;
    int width, height;
    float scale;

    void addGround();
    void addWall(float x, float y, float w, float h);

public:
    PhysicsEngine(int w = 60, int h = 20);

    void addBox(float x, float y, float w, float h);
    void addCircle(float x, float y, float r);
    void step(float timeStep = 1.0f / 60.0f);
    void reset();

    const std::vector<PhysicsBody>& getBodies() const { return bodies; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    float getScale() const { return scale; }
};
