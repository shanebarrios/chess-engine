#pragma once

#include "Bitboard.hpp"
#include "Piece.hpp"

namespace Chess {
	class Position;

	namespace Evaluator {
		int evaluatePiece(PieceType type);
		int evaluatePieces(PieceType type, PieceColor color, Bitboard pieces);
		int evaluate(const Position& position);
	};  // namespace Evaluator
}
