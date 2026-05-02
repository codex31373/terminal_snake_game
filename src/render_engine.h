#pragma once

#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

class RenderEngine {
private:
    int width, height;
    termios oldTio;
    int fdFlags;
    bool initialized;

public:
    RenderEngine(int w, int h);
    ~RenderEngine();

    void init();
    void shutdown();
    void clear() const;
    void moveCursor(int x, int y) const;
    void drawChar(int x, int y, char c) const;
    void drawString(int x, int y, const std::string& str) const;
    void drawBox(int x, int y, int w, int h) const;
    void drawHLine(int x, int y, int length) const;
    void drawVLine(int x, int y, int height) const;
    void hideCursor() const;
    void showCursor() const;
    void flush() const;
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};
