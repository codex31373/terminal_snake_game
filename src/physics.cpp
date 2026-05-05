#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include "render_engine.h"
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>

using Clock = std::chrono::steady_clock;

struct PhysicsBody {
    b2Body* body;
    char symbol;
    float radius;
    bool isCircle;
};

class PhysicsDemo {
private:
    b2World world;
    std::vector<PhysicsBody> bodies;
    RenderEngine& renderer;
    int width, height;
    float scale; // pixels per meter
    bool running;

    void addGround() {
        b2BodyDef groundDef;
        groundDef.position.Set(width / (2.0f * scale), height / scale + 0.5f);
        b2Body* ground = world.CreateBody(&groundDef);

        b2PolygonShape groundBox;
        groundBox.SetAsBox(width / (2.0f * scale), 0.5f);
        ground->CreateFixture(&groundBox, 0.0f);
    }

    void addWall(float x, float y, float w, float h) {
        b2BodyDef wallDef;
        wallDef.position.Set(x, y);
        b2Body* wall = world.CreateBody(&wallDef);

        b2PolygonShape wallBox;
        wallBox.SetAsBox(w, h);
        wall->CreateFixture(&wallBox, 0.0f);
    }

    void addBox(float x, float y, float w, float h) {
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

        bodies.push_back({body, '#', std::max(w, h) * scale, false});
    }

    void addCircle(float x, float y, float r) {
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

        bodies.push_back({body, 'O', r * scale, true});
    }

    void draw() {
        renderer.clear();

        // Draw ground
        int groundY = height + 1;
        renderer.moveCursor(0, groundY);
        for (int i = 0; i < width; ++i) std::cout << "=";

        // Draw bodies
        for (const auto& pb : bodies) {
            b2Vec2 pos = pb.body->GetPosition();
            int screenX = static_cast<int>(pos.x * scale);
            int screenY = static_cast<int>(pos.y * scale);

            if (screenX >= 0 && screenX < width && screenY >= 0 && screenY < height) {
                renderer.moveCursor(screenX, screenY);
                std::cout << pb.symbol;
            }
        }

        renderer.moveCursor(0, height + 2);
        std::cout << "Box2D Physics Demo | Q: Quit | Space: Spawn | R: Reset";
        renderer.flush();
    }

    void handleInput() {
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == ' ') {
                float x = (rand() % width) / scale;
                float y = 1.0f;
                if (rand() % 2 == 0) {
                    addCircle(x, y, 0.3f + (rand() % 50) / 100.0f);
                } else {
                    float w = 0.2f + (rand() % 40) / 100.0f;
                    addBox(x, y, w, w);
                }
            } else if (c == 'r' || c == 'R') {
                reset();
            } else if (c == 'q' || c == 'Q') {
                running = false;
            }
            char flush;
            while (read(STDIN_FILENO, &flush, 1) == 1);
        }
    }

    void reset() {
        for (auto& pb : bodies) {
            world.DestroyBody(pb.body);
        }
        bodies.clear();
        spawnInitial();
    }

    void spawnInitial() {
        addCircle(width / (2.0f * scale), 2.0f, 0.4f);
        addBox(width / (3.0f * scale), 1.0f, 0.3f, 0.3f);
        addBox(width * 2.0f / (3.0f * scale), 3.0f, 0.25f, 0.4f);
    }

public:
    PhysicsDemo(RenderEngine& render, int w = 60, int h = 20)
        : world(b2Vec2(0.0f, 10.0f)), renderer(render), width(w), height(h),
          scale(2.0f), running(true) {
        addGround();
        addWall(-0.5f / scale, height / (2.0f * scale), 0.5f, height / scale);
        addWall(width / scale + 0.5f / scale, height / (2.0f * scale), 0.5f, height / scale);
        spawnInitial();
    }

    void run() {
        const float timeStep = 1.0f / 60.0f;
        const int velocityIterations = 6;
        const int positionIterations = 2;
        const auto frameTime = std::chrono::milliseconds(16);
        auto lastFrame = Clock::now();

        while (running) {
            auto now = Clock::now();
            auto elapsed = now - lastFrame;

            if (elapsed >= frameTime) {
                handleInput();
                world.Step(timeStep, velocityIterations, positionIterations);
                draw();
                lastFrame = now;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
};

int main() {
    std::cout << "Box2D Physics Demo\n";
    std::cout << "Space: Spawn object | R: Reset | Q: Quit\n";
    std::cout << "Press any key to start...\n";
    std::cin.get();

    int screenW = 100, screenH = 30;
    RenderEngine renderer(screenW, screenH);
    renderer.init();

    PhysicsDemo demo(renderer, screenW, screenH);
    demo.run();

    renderer.shutdown();
    std::cout << "Physics demo ended.\n";
    return 0;
}
