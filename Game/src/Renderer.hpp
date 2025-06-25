#pragma once
#include <cstdint>
#include <optional>

class Window;
class Texture;
struct SDL_Renderer;
typedef uint8_t Uint8;

struct Rect {
    int x; int y; int w; int h;
};

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a{ 0xFF };
};

class Renderer {
public:
    Renderer() = default;
    explicit Renderer(Window& window);
    ~Renderer();

    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer& other) = delete;
    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;

    SDL_Renderer* get() const;

    Renderer& present();
    Renderer& clear();
    Renderer& setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xFF);
    Renderer& setDrawColor(Color color);
    Renderer& drawRect(const Rect& rect);
    Renderer& drawRect(int x, int y, int w, int h);
    Renderer& fillRect(const Rect& rect);
    Renderer& fillRect(int x, int y, int w, int h);
    Renderer& drawLine(int x1, int y1, int x2, int y2);
    Renderer& drawTexture(Texture& texture,
        const std::optional<Rect>& src = std::nullopt,
        const std::optional<Rect>& dst = std::nullopt);

private:
    SDL_Renderer* m_renderer{ nullptr };
};