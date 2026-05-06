#pragma once

#include "game.h"
#include "physics_engine.h"

class PhysicsGame : public Game {
private:
    PhysicsEngine engine;
    bool running;

public:
    PhysicsGame(int w = 80, int h = 20);

    void draw(RenderEngine& r) const override;
    void processInput() override;
    void update() override;
    bool shouldQuit() const override;
};
