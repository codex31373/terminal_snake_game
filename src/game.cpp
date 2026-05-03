#include "game.h"
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

SnakeGame::SnakeGame(RenderEngine& render, int w, int h)
    : width(w), height(h), dirX(1), dirY(0), score(0), gameOver(false), paused(false), renderer(render) {
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    
    int startX = width / 2;
    int startY = height / 2;
    snake.push_back({startX, startY});
    snake.push_back({startX - 1, startY});
    snake.push_back({startX - 2, startY});
    
    spawnFood();
}

void SnakeGame::spawnFood() {
    std::uniform_int_distribution<int> distX(0, width - 1);
    std::uniform_int_distribution<int> distY(0, height - 1);
    
    do {
        food = {distX(rng), distY(rng)};
    } while (isSnakeAt(food));
}

bool SnakeGame::isSnakeAt(const Point& p) const {
    for (const auto& s : snake) {
        if (s == p) return true;
    }
    return false;
}

void SnakeGame::handleInput() {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\x1b') {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A': if (dirY != 1) { dirX = 0; dirY = -1; } break;
                        case 'B': if (dirY != -1) { dirX = 0; dirY = 1; } break;
                        case 'C': if (dirX != -1) { dirX = 1; dirY = 0; } break;
                        case 'D': if (dirX != 1) { dirX = -1; dirY = 0; } break;
                    }
                }
            }
        } else {
            switch (c) {
                case 'q': case 'Q': gameOver = true; break;
                case 'p': case 'P': paused = !paused; break;
                case 'w': case 'W': if (dirY != 1) { dirX = 0; dirY = -1; } break;
                case 's': case 'S': if (dirY != -1) { dirX = 0; dirY = 1; } break;
                case 'a': case 'A': if (dirX != -1) { dirX = -1; dirY = 0; } break;
                case 'd': case 'D': if (dirX != 1) { dirX = 1; dirY = 0; } break;
            }
        }
        char flush;
        while (read(STDIN_FILENO, &flush, 1) == 1);
    }
}

void SnakeGame::update() {
    if (paused) return;
    
    Point newHead = {snake[0].x + dirX, snake[0].y + dirY};
    
    if (newHead.x < 0 || newHead.x >= width || newHead.y < 0 || newHead.y >= height) {
        gameOver = true;
        return;
    }
    
    for (size_t i = 0; i < snake.size(); ++i) {
        if (snake[i] == newHead) {
            gameOver = true;
            return;
        }
    }
    
    snake.insert(snake.begin(), newHead);
    
    if (newHead == food) {
        score += 10;
        spawnFood();
    } else {
        snake.pop_back();
    }
}

void SnakeGame::render() {
    renderer.moveCursor(0, 0);
    
    std::cout << "╔";
    for (int i = 0; i < width; ++i) std::cout << "══";
    std::cout << "╗";
    
    for (int y = 0; y < height; ++y) {
        renderer.moveCursor(0, y + 1);
        std::cout << "║";
        for (int x = 0; x < width; ++x) {
            Point p{x, y};
            if (p == snake[0]) {
                std::cout << "██";
            } else if (isSnakeAt(p)) {
                std::cout << "░░";
            } else if (p == food) {
                std::cout << "◆◆";
            } else {
                std::cout << "  ";
            }
        }
        std::cout << "║";
    }
    
    renderer.moveCursor(0, height + 1);
    std::cout << "╚";
    for (int i = 0; i < width; ++i) std::cout << "══";
    std::cout << "╝";
    
    renderer.moveCursor(0, height + 2);
    std::cout << "Score: " << score << "  |  Controls: WASD/Arrows | P: Pause | Q: Quit";
    if (paused) std::cout << " | *** PAUSED ***";
    renderer.flush();
}

void SnakeGame::run() {
    using Clock = std::chrono::steady_clock;
    const auto frameTime = std::chrono::milliseconds(150);
    auto lastFrame = Clock::now();
    
    while (!gameOver) {
        auto now = Clock::now();
        auto elapsed = now - lastFrame;
        
        if (elapsed >= frameTime) {
            handleInput();
            update();
            render();
            lastFrame = now;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
