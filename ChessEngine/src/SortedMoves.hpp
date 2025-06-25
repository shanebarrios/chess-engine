#pragma once

#include "Move.hpp"
#include "Position.hpp"
#include "DataStructures.hpp"

namespace Chess {
    inline constexpr int k_killerScore = INT32_MAX / 2;

    class SortedMoves {
    public:
        SortedMoves(const Position& position, const Array2D<Move, 64, 2>& killerMoves,
            const Array3D<int, 2, 64, 64>& history, Move hashedMove, int depth,
            bool onlyCaptures = false, bool* checkStatus = nullptr);
        Move getNext();
        bool hasNext() const;
        uint8_t size() const;

    private:
        Array<int, 256> m_scores;
        int m_index{ 0 };
        MoveList m_moveList;
    };
}
