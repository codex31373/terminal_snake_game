#pragma once

#include <vector>
#include <random>
#include "types.h"
#include "render_engine.h"

class SnakeGame {
private:
    int width, height;
    std::vector<Point> snake;
    Point food;
    int dirX, dirY;
    int score;
    bool gameOver;
    bool paused;
    
    std::mt19937 rng;
    RenderEngine& renderer;

    void spawnFood();
    bool isSnakeAt(const Point& p) const;
    void handleInput();
    void update();
    void render();

public:
    SnakeGame(RenderEngine& render, int w = 40, int h = 20);
    
    int getScore() const { return score; }
    bool isGameOver() const { return gameOver; }
    
    void run();
};
