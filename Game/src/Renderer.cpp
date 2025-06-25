#include "Renderer.hpp"

#include <SDL2/SDL.h>

#include <stdexcept>

#include "Texture.hpp"
#include "Window.hpp"

Renderer::Renderer(Window& window) {
    m_renderer = SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        throw std::runtime_error{ SDL_GetError() };
    }
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}

Renderer::~Renderer() { SDL_DestroyRenderer(m_renderer); }

Renderer::Renderer(Renderer&& other) noexcept : m_renderer{ other.m_renderer } {
    other.m_renderer = nullptr;
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
    if (&other == this) {
        return *this;
    }
    m_renderer = other.m_renderer;
    other.m_renderer = nullptr;
    return *this;
}

SDL_Renderer* Renderer::get() const { return m_renderer; }

Renderer& Renderer::present() {
    SDL_RenderPresent(m_renderer);
    return *this;
}

Renderer& Renderer::clear() {
    SDL_RenderClear(m_renderer);
    return *this;
}

Renderer& Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    return *this;
}

Renderer& Renderer::setDrawColor(Color color) {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    return *this;
}

Renderer& Renderer::drawRect(const Rect& rect) {
    SDL_Rect _rect{ rect.x, rect.y, rect.w, rect.h };
    SDL_RenderDrawRect(m_renderer, &_rect);
    return *this;
}

Renderer& Renderer::drawRect(int x, int y, int w, int h) {
    SDL_Rect rect{ x, y, w, h };
    SDL_RenderDrawRect(m_renderer, &rect);
    return *this;
}

Renderer& Renderer::fillRect(const Rect& rect) {
    SDL_Rect _rect{ rect.x, rect.y, rect.w, rect.h };
    SDL_RenderFillRect(m_renderer, &_rect);
    return *this;
}

Renderer& Renderer::fillRect(int x, int y, int w, int h) {
    SDL_Rect rect{ x, y, w, h };
    SDL_RenderFillRect(m_renderer, &rect);
    return *this;
}

Renderer& Renderer::drawLine(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(m_renderer, x1, y1, x2, y2);
    return *this;
}

Renderer& Renderer::drawTexture(Texture& texture,
    const std::optional<Rect>& src,
    const std::optional<Rect>& dst) {
    SDL_Rect* _srcPtr = nullptr;
    SDL_Rect* _dstPtr = nullptr;
    SDL_Rect _src;
    SDL_Rect _dst;

    if (src) {
        _src = { src->x, src->y, src->w, src->h };
        _srcPtr = &_src;
    }
    if (dst) {
        _dst = { dst->x, dst->y, dst->w, dst->h };
        _dstPtr = &_dst;
    }
    SDL_RenderCopy(m_renderer, texture.get(), _srcPtr, _dstPtr);
    return *this;
}