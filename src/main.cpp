#include <iostream>
#include "render_engine.h"
#include "game.h"
#include "app.h"

int main() {
    std::cout << "Snake Game - Use WASD or Arrow Keys to move\n";
    std::cout << "Press any key to start...\n";
    std::cin.get();

    RenderEngine renderer(80, 24);
    renderer.init();

    SnakeGame game;
    App app(game, renderer, std::chrono::milliseconds(150));
    app.run();

    renderer.shutdown();
    std::cout << "Game Over! Final Score: " << game.getScore() << "\n";
    return 0;
}
