#pragma once

#include <cstdint>

#include "Piece.hpp"
#include "DataStructures.hpp"

namespace Chess {
    struct Move {
        uint8_t start{};
        uint8_t target{};
        PieceType promotion = PieceType::Null;

        bool operator==(Move other) const {
            return other.start == start && other.target == target &&
                other.promotion == promotion;
        }
    };

    class MoveList {
    public:
        Move& operator[](uint8_t index) { return m_moves[index]; }
        const Move& operator[](uint8_t index) const { return m_moves[index]; }
        void add(Move move) { m_moves[m_length++] = move; }
        void clear() { m_length = 0; }
        uint8_t size() const { return m_length; }

        Move* begin() { return m_moves.data(); }
        Move* end() { return m_moves.data() + m_length; }
        const Move* begin() const { return m_moves.data(); }
        const Move* end() const { return m_moves.data() + m_length; }

    private:
        Array<Move, 256> m_moves;
        uint8_t m_length{ 0 };
    };

}
