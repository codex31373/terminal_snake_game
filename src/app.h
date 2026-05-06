#pragma once

#include "game.h"
#include "draw_layer.h"
#include "input_layer.h"
#include <chrono>

class App {
private:
    Game& game;
    DrawLayer drawLayer;
    InputLayer inputLayer;
    std::chrono::milliseconds frameTime;

public:
    App(Game& g, RenderEngine& r, std::chrono::milliseconds ft);
    void run();
};
