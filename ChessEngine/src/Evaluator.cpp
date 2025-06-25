#include "Evaluator.hpp"

#include <cstdint>

#include "Piece.hpp"
#include "PieceTables.hpp"
#include "Position.hpp"

using namespace Chess;

int Evaluator::evaluatePiece(PieceType piece) {
    static constexpr int evals[] = { 100, 310, 300, 500, 800, 0, 0 };
    return evals[static_cast<uint8_t>(piece)];
}

int Evaluator::evaluatePieces(PieceType type, PieceColor color,
    Bitboard pieces) {
    int score = 0;
    while (pieces) {
        uint8_t square = pieces.popLSB();
        score += evaluatePiece(type) + PieceTables::getValue(type, color, square);
    }
    return score;
}

int Evaluator::evaluate(const Position& position) {
    int score = 0;

    for (uint8_t i = 0; i < 6; i++) {
        const PieceType type = static_cast<PieceType>(i);
        const PieceColor color = position.getTurn();
        score += evaluatePieces(type, color, position.getBitboard(type, color));
    }
    for (uint8_t i = 0; i < 6; i++) {
        const PieceType type = static_cast<PieceType>(i);
        const PieceColor color = position.getOppositeTurn();
        score -= evaluatePieces(type, color, position.getBitboard(type, color));
    }
    return score;
}