#include "Texture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdexcept>
#include <utility>

#include "Renderer.hpp"

Texture::Texture(Renderer& renderer, std::string_view path) {
    loadImage(renderer, path);
}

Texture::~Texture() { SDL_DestroyTexture(m_texture); }
Texture::Texture(Texture&& other) noexcept
    : m_texture{ other.m_texture },
    m_width{ other.m_width },
    m_height{ other.m_height } {
    other.m_texture = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (&other == this) {
        return *this;
    }
    m_texture = other.m_texture;
    m_width = other.m_width;
    m_height = other.m_height;
    other.m_texture = nullptr;
    return *this;
}

SDL_Texture* Texture::get() const { return m_texture; }

int Texture::getWidth() const { return m_width; }
int Texture::getHeight() const { return m_height; }

Texture::operator bool() const {
    return m_texture != nullptr;
}

bool Texture::operator!() const {
    return m_texture == nullptr;
}

Texture& Texture::loadFromSurface(Renderer& renderer, SDL_Surface* surface) {
    SDL_DestroyTexture(m_texture);
    if (!surface) {
        throw std::runtime_error{ IMG_GetError() };
    }
    m_texture = SDL_CreateTextureFromSurface(renderer.get(), surface);
    m_width = surface->w;
    m_height = surface->h;
    SDL_FreeSurface(surface);
    if (!m_texture) {
        throw std::runtime_error{ SDL_GetError() };
    }
    return *this;
}

Texture& Texture::loadImage(Renderer& renderer, std::string_view path) {
    Texture& result{ loadFromSurface(renderer, IMG_Load(path.data())) };
    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
    return result;
}
