#pragma once

#include "render_engine.h"

class IDrawable {
public:
    virtual void draw(RenderEngine& renderer) const = 0;
    virtual ~IDrawable() = default;
};

class DrawLayer {
private:
    RenderEngine& renderer;
public:
    DrawLayer(RenderEngine& r) : renderer(r) {}
    void render(const IDrawable& drawable) const {
        drawable.draw(renderer);
        renderer.flush();
    }
};
