#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

class SnakeGame {
private:
    int width, height;
    std::vector<Point> snake;
    Point food;
    int dirX, dirY;
    int score;
    bool gameOver;
    bool paused;
    termios oldTio;
    int fdFlags;
    
    std::mt19937 rng;

public:
    SnakeGame(int w = 40, int h = 20) : width(w), height(h), dirX(1), dirY(0), score(0), gameOver(false), paused(false) {
        rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
        
        int startX = width / 2;
        int startY = height / 2;
        snake.push_back({startX, startY});
        snake.push_back({startX - 1, startY});
        snake.push_back({startX - 2, startY});
        
        spawnFood();
        setupTerminal();
    }
    
    ~SnakeGame() {
        restoreTerminal();
    }
    
    void setupTerminal() {
        tcgetattr(STDIN_FILENO, &oldTio);
        termios newTio = oldTio;
        newTio.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newTio);
        
        fdFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, fdFlags | O_NONBLOCK);
    }
    
    void restoreTerminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldTio);
        fcntl(STDIN_FILENO, F_SETFL, fdFlags);
    }
    
    void spawnFood() {
        std::uniform_int_distribution<int> distX(0, width - 1);
        std::uniform_int_distribution<int> distY(0, height - 1);
        
        do {
            food = {distX(rng), distY(rng)};
        } while (isSnakeAt(food));
    }
    
    bool isSnakeAt(const Point& p) const {
        for (const auto& s : snake) {
            if (s == p) return true;
        }
        return false;
    }
    
    void handleInput() {
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
        }
    }
    
    void update() {
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
    
    void render() const {
        std::cout << "\033[2J\033[H";
        
        std::cout << "╔";
        for (int i = 0; i < width; ++i) std::cout << "══";
        std::cout << "╗\n";
        
        for (int y = 0; y < height; ++y) {
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
            std::cout << "║\n";
        }
        
        std::cout << "╚";
        for (int i = 0; i < width; ++i) std::cout << "══";
        std::cout << "╝\n";
        
        std::cout << "Score: " << score << "  |  Controls: WASD/Arrows | P: Pause | Q: Quit\n";
        if (paused) std::cout << "*** PAUSED ***\n";
    }
    
    void run() {
        while (!gameOver) {
            handleInput();
            update();
            render();
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        
        std::cout << "\nGame Over! Final Score: " << score << "\n";
    }
};

int main() {
    std::cout << "Snake Game - Use WASD or Arrow Keys to move\n";
    std::cout << "Press any key to start...\n";
    std::cin.get();
    
    SnakeGame game;
    game.run();
    
    return 0;
}
