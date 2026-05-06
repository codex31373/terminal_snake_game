#include "app.h"
#include <thread>

using Clock = std::chrono::steady_clock;

App::App(Game& g, RenderEngine& r, std::chrono::milliseconds ft)
    : game(g), drawLayer(r), inputLayer(), frameTime(ft) {}

void App::run() {
    auto lastFrame = Clock::now();
    while (true) {
        auto now = Clock::now();
        auto elapsed = now - lastFrame;
        if (elapsed >= frameTime) {
            if (!inputLayer.process(game)) break;
            game.update();
            drawLayer.render(game);
            lastFrame = now;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
