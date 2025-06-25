#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <string_view>

#include "Move.hpp"
#include "Piece.hpp"
#include "Position.hpp"

namespace Chess {
    namespace Utils {

        struct Coordinate {
            int row;
            int col;

            static Coordinate fromSquare(uint8_t square) {
                return Coordinate{ square / 8, square % 8 };
            }

            uint8_t toSquare() const { return row * 8 + col; }

            bool inBounds() const { return row >= 0 && row < 8 && col >= 0 && col < 8; }
            Coordinate operator+(Coordinate other) const {
                return Coordinate{ row + other.row, col + other.col };
            }
            Coordinate operator-(Coordinate other) const {
                return Coordinate{ row - other.row, col - other.col };
            }
            void operator+=(Coordinate other) {
                row += other.row;
                col += other.col;
            }
            void operator-=(Coordinate other) {
                row -= other.row;
                col -= other.col;
            }
        };

        inline constexpr Coordinate knightDirections[8]{
            {-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2} };

        inline constexpr Coordinate slidingDirections[8]{
            {-1, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, 1}, {1, 1}, {1, -1}, {-1, -1} };


        std::string squareToStr(uint8_t square);

        std::string moveToStr(Move move);

        Move strToMove(std::string_view strMove);

        char pieceToChar(Piece piece);

        PieceType charToPieceType(char c);

        std::vector<Bitboard> getSubsets(Bitboard board);

        std::string bitboardToStr(Bitboard board);

        std::vector<std::string> split(std::string_view str, char delimeter = ' ');

        void positionDebugPrint(const Position& position);
    }  // namespace Utils
}
