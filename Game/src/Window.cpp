#include "Window.hpp"

#include <SDL.h>

Window::Window(std::string_view title, int width, int height) {
    init(title, width, height);
}

Window::~Window() { SDL_DestroyWindow(m_window); }

Window::Window(Window&& other) noexcept : m_window{ other.m_window } {
    other.m_window = nullptr;
}

Window& Window::operator=(Window&& other) noexcept {
    if (&other == this) {
        return *this;
    }
    SDL_DestroyWindow(m_window);
    m_window = other.m_window;
    other.m_window = nullptr;
    return *this;
}

SDL_Window* Window::get() { return m_window; }

Window& Window::setSize(int w, int h) {
    SDL_SetWindowSize(m_window, w, h);
    return *this;
}

Window& Window::show() {
    SDL_ShowWindow(m_window);
    return *this;
}

Window& Window::hide() {
    SDL_HideWindow(m_window);
    return *this;
}

void Window::init(std::string_view title = "Window", int width = 0,
    int height = 0) {
    m_window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_HIDDEN);
    if (!m_window) {
        throw std::runtime_error(SDL_GetError());
    }
}