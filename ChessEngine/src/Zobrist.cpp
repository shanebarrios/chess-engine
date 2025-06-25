#include "Zobrist.hpp"

#include <cstdint>
#include <random>

#include "Piece.hpp"
#include "Position.hpp"

using namespace Chess;

namespace {
    bool initialized = false;

    constexpr size_t k_hashArrLength = 12 * 64 + 1 + 16 + 8;
    uint64_t hashes[k_hashArrLength];

    constexpr size_t k_pieceOffset = 0;
    constexpr size_t k_blackToMoveOffset = k_pieceOffset + 64 * 12;
    constexpr size_t k_castlingFlagsOffset = k_blackToMoveOffset + 1;
    constexpr size_t k_enPassantFileOffset = k_castlingFlagsOffset + 16;

    uint64_t hashEnPassantFile(uint8_t file) {
        return hashes[k_enPassantFileOffset + file];
    }

    uint64_t hashCastlingFlags(uint8_t flags) {
        return hashes[k_castlingFlagsOffset + flags];
    }

    uint64_t hashBlackToMove() { return hashes[k_blackToMoveOffset]; }

    uint64_t hashPiece(PieceType type, PieceColor color, uint8_t square) {
        return hashes[k_pieceOffset + square * 12 + static_cast<uint8_t>(color) * 6 +
            static_cast<uint8_t>(type)];
    }

}  // namespace

Zobrist Zobrist::fromPosition(const Position& position) {
    uint64_t hash = 0;
    for (uint8_t i = 0; i < 6; i++) {
        for (uint8_t j = 0; j < 2; j++) {
            Bitboard bb = position.getBitboard(static_cast<PieceType>(i),
                static_cast<PieceColor>(j));
            while (bb) {
                const uint8_t square = bb.popLSB();
                hash ^= hashPiece(static_cast<PieceType>(i), static_cast<PieceColor>(j),
                    square);
            }
        }
    }
    if (position.canEnPassant()) {
        const uint8_t file = position.getEnPassantTarget() % 8;
        hash ^= hashEnPassantFile(file);
    }
    if (position.getTurn() == PieceColor::Black) {
        hash ^= hashBlackToMove();
    }
    hash ^= hashCastlingFlags(position.getCastlingFlags());
    return Zobrist{ hash };
}

void Zobrist::togglePiece(PieceType type, PieceColor color, uint8_t square) {
    m_hash ^= hashPiece(type, color, square);
}

void Zobrist::toggleSide() { m_hash ^= hashBlackToMove(); }

void Zobrist::toggleCastlingFlags(uint8_t flags) {
    m_hash ^= hashCastlingFlags(flags);
}

void Zobrist::toggleEnPassantFile(uint8_t file) {
    m_hash ^= hashEnPassantFile(file);
}

void Zobrist::init() {
    if (initialized) {
        return;
    }
    std::mt19937 mt{ 1 };
    std::uniform_int_distribution<uint64_t> gen(0, UINT64_MAX);
    for (int i = 0; i < k_hashArrLength; i++) {
        hashes[i] = gen(mt);
    }
    initialized = true;
}