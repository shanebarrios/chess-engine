#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "Renderer.hpp"
#include "Texture.hpp"
#include "Window.hpp"

inline void graphicsInit() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error{ SDL_GetError() };
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        throw std::runtime_error{ IMG_GetError() };
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
}

inline void graphicsCleanup() {
    IMG_Quit();
    SDL_Quit();
}