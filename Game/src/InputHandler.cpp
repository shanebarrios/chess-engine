#include "InputHandler.hpp"
#include <algorithm>
#include <SDL_events.h>
#include "IMouseListener.hpp"

InputHandler& InputHandler::instance() {
    static InputHandler instance{};
    return instance;
}

void InputHandler::handle() {
    SDL_Event e;
    SDL_WaitEvent(&e);
    int x, y;
    SDL_GetMouseState(&x, &y);
    switch (e.type) {
    case SDL_QUIT:
        m_shouldQuit = true;
        break;
    case SDL_MOUSEMOTION:
        notifyMouseMove(x, y);
        break;
    case SDL_MOUSEBUTTONDOWN:
        notifyMousePress(x, y);
        break;
    case SDL_MOUSEBUTTONUP:
        notifyMouseRelease(x, y);
    }
}

void InputHandler::nullPush() {
    SDL_Event e;
    SDL_zero(e);
    SDL_PushEvent(&e);
}

bool InputHandler::shouldQuit() const {
    return m_shouldQuit;
}

void InputHandler::addListener(IMouseListener* listener) {
    m_listeners.push_back(listener);
}

void InputHandler::removeListener(IMouseListener* listener) {
    m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
}

void InputHandler::notifyMouseMove(int x, int y) {
    for (IMouseListener* listener : m_listeners) {
        listener->onMouseMove(x, y);
    }
}

void InputHandler::notifyMousePress(int x, int y) {
    for (IMouseListener* listener : m_listeners) {
        listener->onMousePress(x, y);
    }
}

void InputHandler::notifyMouseRelease(int x, int y) {
    for (IMouseListener* listener : m_listeners) {
        listener->onMouseRelease(x, y);
    }
}