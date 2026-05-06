#include "physics_game.h"
#include <iostream>
#include <unistd.h>

PhysicsGame::PhysicsGame(int w, int h)
    : engine(w, h), running(true) {
    engine.addCircle(w / (2.0f * 2.0f), 2.0f, 0.4f);
    engine.addBox(w / (3.0f * 2.0f), 1.0f, 0.3f, 0.3f);
    engine.addBox(w * 2.0f / (3.0f * 2.0f), 3.0f, 0.25f, 0.4f);
}

void PhysicsGame::draw(RenderEngine& renderer) const {
    renderer.clear();

    int h = engine.getHeight();
    int w = engine.getWidth();

    renderer.moveCursor(0, h + 1);
    for (int i = 0; i < w; ++i) std::cout << "=";

    for (const auto& pb : engine.getBodies()) {
        b2Vec2 pos = pb.body->GetPosition();
        float scale = engine.getScale();
        int screenX = static_cast<int>(pos.x * scale);
        int screenY = static_cast<int>(pos.y * scale);

        if (screenX >= 0 && screenX < w && screenY >= 0 && screenY < h) {
            renderer.moveCursor(screenX, screenY);
            std::cout << pb.symbol;
        }
    }

    renderer.moveCursor(0, h + 2);
    std::cout << "Box2D Physics Demo | Q: Quit | Space: Spawn | R: Reset";
}

void PhysicsGame::processInput() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == ' ') {
            int w = engine.getWidth();
            float scale = engine.getScale();
            float x = (rand() % w) / scale;
            float y = 1.0f;
            if (rand() % 2 == 0) {
                engine.addCircle(x, y, 0.3f + (rand() % 50) / 100.0f);
            } else {
                float sz = 0.2f + (rand() % 40) / 100.0f;
                engine.addBox(x, y, sz, sz);
            }
        } else if (c == 'r' || c == 'R') {
            engine.reset();
        } else if (c == 'q' || c == 'Q') {
            running = false;
        }
        char flush;
        while (read(STDIN_FILENO, &flush, 1) == 1);
    }
}

void PhysicsGame::update() {
    engine.step();
}

bool PhysicsGame::shouldQuit() const {
    return !running;
}
