#pragma once

#include <cstdint>
#include <functional>

#include "Piece.hpp"

namespace Chess {
	class Position;

	class Zobrist {
	public:
		static void init();

		static Zobrist fromPosition(const Position& position);

		Zobrist() = default;
		explicit Zobrist(uint64_t hash) : m_hash{ hash } {}

		bool operator==(Zobrist other) const { return m_hash == other.m_hash; }
		bool operator!=(Zobrist other) const { return m_hash != other.m_hash; }
		explicit operator uint64_t() const { return m_hash; }

		void togglePiece(PieceType type, PieceColor color, uint8_t square);
		void toggleSide();
		void toggleCastlingFlags(uint8_t flags);
		void toggleEnPassantFile(uint8_t file);

		uint64_t get() const { return m_hash; }

	private:
		uint64_t m_hash{};
	};

}
