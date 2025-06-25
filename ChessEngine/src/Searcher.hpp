#pragma once

#include <utility>

#include "Move.hpp"
#include "Transposition.hpp"
#include "DataStructures.hpp"

namespace Chess {
    class Position;

    class Searcher {
    public:
        Searcher() = default;
        Move getMove(const Position& position, int thinkMilliseconds = 1000);

    private:
        TranspositionTable m_transpositionTable{};
        Array2D<Move, 64, 2> m_killerMoves{};
        Array3D<int, 2, 64, 64> m_history{};

        bool m_timeUp{ false };
        int m_nodes{ 0 };
        int m_transpositions{ 0 };

        std::pair<Move, int> rootSearch(Position& position, int depth);
        int search(Position& position, int depth, int ply, int alpha, int beta,
            bool isPV);
        int quiescenceSearch(Position& position, int alpha, int beta);
    };

}
