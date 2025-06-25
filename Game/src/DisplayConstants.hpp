#pragma once

#include "Renderer.hpp"

namespace DisplayConstants {
    inline constexpr int k_windowWidth = 1280;
    inline constexpr int k_windowHeight = 720;
    inline constexpr int k_boardMargin = 20;
    inline constexpr int k_numRanks = 8;
    inline constexpr int k_squareWidth = (k_windowHeight - 2 * k_boardMargin) / k_numRanks;
    inline constexpr int k_boardWidth = k_squareWidth * k_numRanks;
    inline constexpr int k_boardStartX = (k_windowWidth - k_boardWidth) / 2;
    inline constexpr int k_boardStartY = k_boardMargin;

    inline constexpr Color darkSquareColor{ 0x76, 0x96, 0x56 };
    inline constexpr Color lightSquareColor{ 0xee, 0xee, 0xd2 };
    inline constexpr Color legalMoveColor{ 0xff, 0x00, 0x00, 0xaa };
    inline constexpr Color boardColors[]{ lightSquareColor, darkSquareColor };
}