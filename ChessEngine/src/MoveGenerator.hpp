#pragma once

#include <vector>

#include "Move.hpp"

namespace Chess {
    class Position;

    namespace MoveGenerator {
        // returns bool indicating whether in check (not clean but efficient)
        bool generateLegal(const Position& position, MoveList& legalMoves,
            bool onlyCaptures = false);
    }  // namespace MoveGenerator
}
