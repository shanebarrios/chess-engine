#pragma once

#include <vector>

class IMouseListener;

class InputHandler {
public:
    static InputHandler& instance();
    void handle();

    void addListener(IMouseListener* listener);
    void removeListener(IMouseListener* listener);

    // handle() is blocking, so nullPush() just wakes it up
    void nullPush();

    bool shouldQuit() const;
private:
    InputHandler() {}

    std::vector<IMouseListener*> m_listeners{};
    bool m_shouldQuit{ false };

    void notifyMouseMove(int x, int y);
    void notifyMouseRelease(int x, int y);
    void notifyMousePress(int x, int y);
};