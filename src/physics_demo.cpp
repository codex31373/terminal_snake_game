#include <iostream>
#include <thread>
#include <chrono>
#include "render_engine.h"
#include "physics_engine.h"
#include <unistd.h>

using Clock = std::chrono::steady_clock;

class PhysicsDemo {
private:
    PhysicsEngine& engine;
    RenderEngine& renderer;
    bool running;

    void draw() {
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
        renderer.flush();
    }

    void handleInput() {
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

public:
    PhysicsDemo(PhysicsEngine& phys, RenderEngine& render)
        : engine(phys), renderer(render), running(true) {}

    void run() {
        const auto frameTime = std::chrono::milliseconds(16);
        auto lastFrame = Clock::now();

        while (running) {
            auto now = Clock::now();
            auto elapsed = now - lastFrame;

            if (elapsed >= frameTime) {
                handleInput();
                engine.step();
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

    int screenW = 80, screenH = 20;
    RenderEngine renderer(screenW, screenH);
    renderer.init();

    PhysicsEngine engine(screenW, screenH);
    engine.addCircle(screenW / (2.0f * 2.0f), 2.0f, 0.4f);
    engine.addBox(screenW / (3.0f * 2.0f), 1.0f, 0.3f, 0.3f);
    engine.addBox(screenW * 2.0f / (3.0f * 2.0f), 3.0f, 0.25f, 0.4f);

    PhysicsDemo demo(engine, renderer);
    demo.run();

    renderer.shutdown();
    std::cout << "Physics demo ended.\n";
    return 0;
}
