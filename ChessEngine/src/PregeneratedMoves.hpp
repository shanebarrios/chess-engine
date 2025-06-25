#pragma once

#include "Bitboard.hpp"

// Maybe should be singleton class instead of namespace because it stores state, idk 
namespace Chess {
	namespace PregeneratedMoves {

		void init();

		Bitboard getRookMoves(uint8_t square, Bitboard occupied);
		Bitboard getBishopMoves(uint8_t square, Bitboard occupied);
		Bitboard getQueenMoves(uint8_t square, Bitboard occupied);
		Bitboard getKnightMoves(uint8_t square);
		Bitboard getKingMoves(uint8_t square);
		Bitboard getBetween(uint8_t src, uint8_t dst);
		Bitboard getLine(uint8_t src, uint8_t dst);
	} 
}