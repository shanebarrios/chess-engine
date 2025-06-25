#pragma once

#include "Bitboard.hpp"
#include "Searcher.hpp"
#include "Move.hpp"
#include "Position.hpp"
#include "Utils.hpp"
#include "Piece.hpp"
#include "Zobrist.hpp"
#include "PregeneratedMoves.hpp"
#include "MoveGenerator.hpp"
#include "SquareAliases.hpp"

namespace Chess {
	inline void init() {
		Zobrist::init();
		PregeneratedMoves::init();
	}
}
