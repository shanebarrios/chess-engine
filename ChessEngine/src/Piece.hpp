#pragma once

#include <cstdint>

namespace Chess {
	enum class PieceType : uint8_t {
		Pawn = 0,
		Bishop,
		Knight,
		Rook,
		Queen,
		King,
		Null
	};

	enum class PieceColor : uint8_t { White = 0, Black };

	struct Piece {
		PieceType type = PieceType::Null;
		PieceColor color = PieceColor::White;

		operator bool() const { return type != PieceType::Null; }
		bool operator!() const { return type == PieceType::Null; }
	};
}
