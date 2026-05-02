#include "render_engine.h"

RenderEngine::RenderEngine(int w, int h) : width(w), height(h), initialized(false) {}

RenderEngine::~RenderEngine() {
    if (initialized) shutdown();
}

void RenderEngine::init() {
    if (initialized) return;
    
    tcgetattr(STDIN_FILENO, &oldTio);
    termios newTio = oldTio;
    newTio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newTio);
    
    fdFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, fdFlags | O_NONBLOCK);
    
    std::cout << "\033[?1049h";
    hideCursor();
    clear();
    
    initialized = true;
}

void RenderEngine::shutdown() {
    if (!initialized) return;
    
    showCursor();
    std::cout << "\033[?1049l";
    flush();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTio);
    fcntl(STDIN_FILENO, F_SETFL, fdFlags);
    
    initialized = false;
}

void RenderEngine::clear() const {
    std::cout << "\033[H\033[J";
}

void RenderEngine::moveCursor(int x, int y) const {
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
}

void RenderEngine::drawChar(int x, int y, char c) const {
    moveCursor(x, y);
    std::cout << c;
}

void RenderEngine::drawString(int x, int y, const std::string& str) const {
    moveCursor(x, y);
    std::cout << str;
}

void RenderEngine::drawBox(int x, int y, int w, int h) const {
    moveCursor(x, y);
    std::cout << "┌";
    for (int i = 0; i < w - 2; ++i) std::cout << "─";
    std::cout << "┐";
    
    for (int row = 1; row < h - 1; ++row) {
        moveCursor(x, y + row);
        std::cout << "│";
        moveCursor(x + w - 1, y + row);
        std::cout << "│";
    }
    
    moveCursor(x, y + h - 1);
    std::cout << "└";
    for (int i = 0; i < w - 2; ++i) std::cout << "─";
    std::cout << "┘";
}

void RenderEngine::drawHLine(int x, int y, int length) const {
    moveCursor(x, y);
    for (int i = 0; i < length; ++i) std::cout << "─";
}

void RenderEngine::drawVLine(int x, int y, int h) const {
    for (int i = 0; i < h; ++i) {
        moveCursor(x, y + i);
        std::cout << "│";
    }
}

void RenderEngine::hideCursor() const {
    std::cout << "\033[?25l";
}

void RenderEngine::showCursor() const {
    std::cout << "\033[?25h";
}

void RenderEngine::flush() const {
    std::cout << std::flush;
}
