#pragma once

class IInputHandler {
public:
    virtual void processInput() = 0;
    virtual bool shouldQuit() const = 0;
    virtual ~IInputHandler() = default;
};

class InputLayer {
public:
    bool process(IInputHandler& handler) const {
        handler.processInput();
        return !handler.shouldQuit();
    }
};
