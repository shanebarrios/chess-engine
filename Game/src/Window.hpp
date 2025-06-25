#pragma once

#include <stdexcept>
#include <string_view>

struct SDL_Window;

class Window {
public:
    explicit Window(std::string_view title = "Window", int width = 0,
        int height = 0);
    ~Window();

    Window(const Window& other) = delete;
    Window& operator=(const Window& other) = delete;
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    SDL_Window* get();

    Window& setSize(int w, int h);
    Window& show();
    Window& hide();

private:
    SDL_Window* m_window{};

    void init(std::string_view title, int width, int height);
};