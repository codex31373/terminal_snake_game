#include <iostream>
#include "render_engine.h"
#include "physics_game.h"
#include "app.h"

int main() {
    std::cout << "Box2D Physics Demo\n";
    std::cout << "Space: Spawn object | R: Reset | Q: Quit\n";
    std::cout << "Press any key to start...\n";
    std::cin.get();

    int screenW = 150, screenH = 45;
    RenderEngine renderer(screenW, screenH);
    renderer.init();

    PhysicsGame game(screenW, screenH);
    App app(game, renderer, std::chrono::milliseconds(16));
    app.run();

    renderer.shutdown();
    std::cout << "Physics demo ended.\n";
    return 0;
}
