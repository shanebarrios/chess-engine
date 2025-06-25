#include "PregeneratedMoves.hpp"

#include <unordered_set>
#include <vector>

#include "MagicData.hpp"
#include "Utils.hpp"
#include "DataStructures.hpp"

using namespace Chess;

namespace {
    Array<Bitboard, 64> knightMoves{};
    Array<Bitboard, 64> kingMoves{};
    Array2D<Bitboard, 64, 512> bishopMoves{};
    Array2D<Bitboard, 64, 4096> rookMoves{};
    // ray from i -> j, not inclusive of i
    Array2D<Bitboard, 64, 64> between{};
    // ray from i through j to end of board, not inclusive of i
    Array2D<Bitboard, 64, 64> lines{};

    Bitboard getSlidingMoves(uint8_t square, Bitboard blockers, bool rook) {
        Bitboard moves{};
        const uint8_t start = rook ? 0 : 4;
        const uint8_t end = rook ? 3 : 7;

        for (uint8_t dirIndex = start; dirIndex <= end; dirIndex++) {
            const Utils::Coordinate dcoord = Utils::slidingDirections[dirIndex];
            auto coord = Utils::Coordinate::fromSquare(square) + dcoord;

            while (coord.inBounds()) {
                const auto board = Bitboard::fromSquare(coord.toSquare());
                moves |= board;
                if (board & blockers) {
                    break;
                }
                coord += dcoord;
            }
        }
        return moves;
    }

    Bitboard genKnightMoves(uint8_t square) {
        Bitboard moves{};

        for (uint8_t dirIndex = 0; dirIndex < 8; dirIndex++) {
            const Utils::Coordinate coord = Utils::Coordinate::fromSquare(square) +
                Utils::knightDirections[dirIndex];

            if (coord.inBounds()) {
                moves.setBit(coord.toSquare());
            }
        }
        return moves;
    }

    Bitboard genKingMoves(uint8_t square) {
        Bitboard moves{};

        for (uint8_t dirIndex = 0; dirIndex < 8; dirIndex++) {
            const Utils::Coordinate coord = Utils::Coordinate::fromSquare(square) +
                Utils::slidingDirections[dirIndex];

            if (coord.inBounds()) {
                moves.setBit(coord.toSquare());
            }
        }
        return moves;
    }

    void setBetween(uint8_t square) {
        for (uint8_t dirIndex = 0; dirIndex < 8; dirIndex++) {
            const Utils::Coordinate dcoord = Utils::slidingDirections[dirIndex];
            Utils::Coordinate coord = Utils::Coordinate::fromSquare(square) + dcoord;
            Bitboard board{};
            while (coord.inBounds()) {
                board.setBit(coord.toSquare());
                between[square][coord.toSquare()] = board;
                coord += dcoord;
            }
        }
    }

    void setLine(uint8_t square) {
        for (uint8_t dirIndex = 0; dirIndex < 8; dirIndex++) {
            const Utils::Coordinate dcoord = Utils::slidingDirections[dirIndex];
            Utils::Coordinate coord = Utils::Coordinate::fromSquare(square) + dcoord;
            Bitboard board = Bitboard::fromSquare(square);
            std::unordered_set<uint8_t> visited{};
            while (coord.inBounds()) {
                board.setBit(coord.toSquare());
                visited.insert(coord.toSquare());
                coord += dcoord;
            }
            coord = Utils::Coordinate::fromSquare(square) - dcoord;
            while (coord.inBounds()) {
                board.setBit(coord.toSquare());
                visited.insert(coord.toSquare());
                coord -= dcoord;
            }
            for (uint8_t dst : visited) {
                lines[square][dst] = board;
            }
        }
    }

    uint64_t magicIndex(const MagicEntry& magicEntry, Bitboard blockers) {
        return (blockers * magicEntry.magic) >> (magicEntry.shifts);
    }

}  // namespace

Bitboard PregeneratedMoves::getRookMoves(uint8_t square, Bitboard occupied) {
    const MagicEntry& entry = MagicData::rookMagics[square];
    Bitboard blockers = occupied & entry.mask;
    uint64_t ind = magicIndex(entry, blockers);
    return rookMoves[square][ind];
}

Bitboard PregeneratedMoves::getBishopMoves(uint8_t square, Bitboard occupied) {
    const MagicEntry& entry = MagicData::bishopMagics[square];
    Bitboard blockers = occupied & entry.mask;
    uint64_t ind = magicIndex(entry, blockers);
    return bishopMoves[square][ind];
}

Bitboard PregeneratedMoves::getQueenMoves(uint8_t square, Bitboard occupied) {
    return getRookMoves(square, occupied) | getBishopMoves(square, occupied);
}

Bitboard PregeneratedMoves::getKnightMoves(uint8_t square) {
    return knightMoves[square];
}

Bitboard PregeneratedMoves::getKingMoves(uint8_t square) {
    return kingMoves[square];
}

Bitboard PregeneratedMoves::getBetween(uint8_t src, uint8_t dst) {
    return between[src][dst];
}

Bitboard PregeneratedMoves::getLine(uint8_t src, uint8_t dst) {
    return lines[src][dst];
}

void PregeneratedMoves::init() {
    for (uint8_t square = 0; square < 64; square++) {
        knightMoves[square] = genKnightMoves(square);
        kingMoves[square] = genKingMoves(square);

        const MagicEntry& rookMagic = MagicData::rookMagics[square];
        std::vector<Bitboard> rookSubsets = Utils::getSubsets(rookMagic.mask);
        for (Bitboard board : rookSubsets) {
            uint64_t index = magicIndex(rookMagic, board);
            rookMoves[square][index] = getSlidingMoves(square, board, true);
        }

        const MagicEntry& bishopMagic = MagicData::bishopMagics[square];
        std::vector<Bitboard> bishopSubsets = Utils::getSubsets(bishopMagic.mask);
        for (Bitboard board : bishopSubsets) {
            uint64_t index = magicIndex(bishopMagic, board);
            bishopMoves[square][index] = getSlidingMoves(square, board, false);
        }

        setBetween(square);
        setLine(square);
    }
}