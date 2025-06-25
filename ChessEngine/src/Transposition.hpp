#pragma once

#include <iostream>
#include <optional>

#include "Move.hpp"
#include "Zobrist.hpp"
#include "DataStructures.hpp"

namespace Chess {
    class Position;

    inline constexpr int k_infinity = 1000000000;

    struct TranspositionEntry {
        enum Flag : uint8_t { Exact, Lower, Upper };

        Zobrist key;
        Move move;
        uint8_t depth;
        int score;
        Flag flag;
    };

    class TranspositionTable {
    public:
        TranspositionTable() = default;

        void tryStore(const Position& position, Move move, int depth, int score,
            TranspositionEntry::Flag flag);

        std::optional<int> probeScore(const Position& position, int depth, int ply,
            int alpha, int beta);

        Move probeMove(const Position& position);

    private:
        HeapArray<TranspositionEntry, 1u << 20> m_table{};
    };
}
