#pragma once

#include "draw_layer.h"
#include "input_layer.h"
#include <vector>
#include <random>
#include "types.h"

class Game : public IDrawable, public IInputHandler {
public:
    virtual void update() = 0;
    virtual ~Game() = default;
};

class SnakeGame : public Game {
private:
    int width, height;
    std::vector<Point> snake;
    Point food;
    int dirX, dirY;
    int score;
    bool gameOver;
    bool paused;
    std::mt19937 rng;

    void spawnFood();
    bool isSnakeAt(const Point& p) const;

public:
    SnakeGame(int w = 40, int h = 20);

    void draw(RenderEngine& r) const override;
    void processInput() override;
    void update() override;
    bool shouldQuit() const override;

    int getScore() const { return score; }
};
