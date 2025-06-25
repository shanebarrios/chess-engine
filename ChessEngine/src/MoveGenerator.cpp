
#include "MoveGenerator.hpp"

#include <iostream>
#include <vector>

#include "Move.hpp"
#include "Position.hpp"
#include "PregeneratedMoves.hpp"
#include "SquareAliases.hpp"
#include "Utils.hpp"

using namespace Chess;

namespace {
    Bitboard getDangerSquares(const Position& position) {
        Bitboard dangers{};
        const Bitboard occupied =
            position.getOccupied() &
            ~position.getBitboard(PieceType::King, position.getTurn());

        Bitboard orthogonal = position.getOpponentOrthogonal();
        while (orthogonal) {
            const uint8_t square = orthogonal.popLSB();
            dangers |= PregeneratedMoves::getRookMoves(square, occupied);
        }

        Bitboard diagonal = position.getOpponentDiagonal();
        while (diagonal) {
            const uint8_t square = diagonal.popLSB();
            dangers |= PregeneratedMoves::getBishopMoves(square, occupied);
        }

        Bitboard knights = position.getOpponentKnights();
        while (knights) {
            const uint8_t square = knights.popLSB();
            dangers |= PregeneratedMoves::getKnightMoves(square);
        }

        const Bitboard pawns = position.getOpponentPawns();
        if (position.getTurn() == PieceColor::White) {
            dangers |= pawns.southEast();
            dangers |= pawns.southWest();
        }
        else {
            dangers |= pawns.northEast();
            dangers |= pawns.northWest();
        }

        dangers |= PregeneratedMoves::getKingMoves(position.getOpponentKingSquare());

        return dangers;
    }

    Bitboard getAttackers(const Position& position) {
        const uint8_t kingSquare = position.getFriendlyKingSquare();
        const Bitboard occupied = position.getOccupied();

        Bitboard attackers{};

        const Bitboard knights = position.getOpponentKnights();
        attackers |= PregeneratedMoves::getKnightMoves(kingSquare) & knights;

        const Bitboard orthogonal = position.getOpponentOrthogonal();
        attackers |=
            PregeneratedMoves::getRookMoves(kingSquare, occupied) & orthogonal;

        const Bitboard diagonal = position.getOpponentDiagonal();
        attackers |=
            PregeneratedMoves::getBishopMoves(kingSquare, occupied) & diagonal;

        const Bitboard pawns = position.getOpponentPawns();
        const Bitboard king = Bitboard::fromSquare(kingSquare);
        if (position.getTurn() == PieceColor::White) {
            attackers |= king.northEast() & pawns;
            attackers |= king.northWest() & pawns;
        }
        else {
            attackers |= king.southEast() & pawns;
            attackers |= king.southWest() & pawns;
        }

        return attackers;
    }

    Bitboard getPinned(const Position& position) {
        const uint8_t kingSquare = position.getFriendlyKingSquare();
        const Bitboard occupied = position.getOccupied();
        const Bitboard friendly = position.getFriendlyBitboard();
        Bitboard pinned{};

        const Bitboard rookMovesFromKing =
            PregeneratedMoves::getRookMoves(kingSquare, occupied);
        Bitboard orthogonal = position.getOpponentOrthogonal();
        while (orthogonal) {
            const uint8_t square = orthogonal.popLSB();
            const Bitboard moves = PregeneratedMoves::getRookMoves(square, occupied);
            pinned |= moves & rookMovesFromKing & friendly &
                PregeneratedMoves::getLine(kingSquare, square);
        }

        const Bitboard bishopMovesFromKing =
            PregeneratedMoves::getBishopMoves(kingSquare, occupied);
        Bitboard diagonal = position.getOpponentDiagonal();
        while (diagonal) {
            const uint8_t square = diagonal.popLSB();
            const Bitboard moves = PregeneratedMoves::getBishopMoves(square, occupied);
            pinned |= moves & bishopMovesFromKing & friendly &
                PregeneratedMoves::getLine(kingSquare, square);
        }
        return pinned;
    }

    void addBitboardMoves(MoveList& legalMoves, Bitboard bitboard, uint8_t start) {
        while (bitboard) {
            uint8_t target = bitboard.popLSB();
            legalMoves.add({ start, target });
        }
    }

    // ignore castling for now
    void addKingMoves(MoveList& legalMoves, const Position& position,
        Bitboard dangerSquares, bool onlyCaptures) {
        const uint8_t kingSquare = position.getFriendlyKingSquare();
        const Bitboard friendly = position.getFriendlyBitboard();
        Bitboard kingMoves =
            PregeneratedMoves::getKingMoves(kingSquare) & ~dangerSquares & ~friendly;
        if (onlyCaptures) {
            kingMoves &= position.getOpponentBitboard();
        }
        addBitboardMoves(legalMoves, kingMoves, kingSquare);
    }

    void addSlidingMoves(MoveList& legalMoves, const Position& position,
        Bitboard checkMask, Bitboard pinned, bool onlyCaptures) {
        Bitboard mask = checkMask & ~position.getFriendlyBitboard();
        if (onlyCaptures) {
            mask &= position.getOpponentBitboard();
        }
        const uint8_t kingSquare = position.getFriendlyKingSquare();

        Bitboard orthogonal = position.getFriendlyOrthogonal();
        while (orthogonal) {
            const uint8_t square = orthogonal.popLSB();
            Bitboard moves =
                PregeneratedMoves::getRookMoves(square, position.getOccupied()) & mask;
            if (pinned.checkBit(square)) {
                moves &= PregeneratedMoves::getLine(kingSquare, square);
            }
            addBitboardMoves(legalMoves, moves, square);
        }

        Bitboard diagonal = position.getFriendlyDiagonal();
        while (diagonal) {
            const uint8_t square = diagonal.popLSB();
            Bitboard moves =
                PregeneratedMoves::getBishopMoves(square, position.getOccupied()) &
                mask;
            if (pinned.checkBit(square)) {
                moves &= PregeneratedMoves::getLine(kingSquare, square);
            }
            addBitboardMoves(legalMoves, moves, square);
        }
    }

    void addKnightMoves(MoveList& legalMoves, const Position& position,
        Bitboard checkMask, Bitboard pinned, bool onlyCaptures) {
        Bitboard mask = checkMask & ~position.getFriendlyBitboard();
        if (onlyCaptures) {
            mask &= position.getOpponentBitboard();
        }
        Bitboard knights =
            position.getFriendlyKnights() & ~pinned;  // pinned knights cant move
        while (knights) {
            const uint8_t square = knights.popLSB();
            Bitboard moves = PregeneratedMoves::getKnightMoves(square) & mask;
            addBitboardMoves(legalMoves, moves, square);
        }
    }

    void tryAddPawnMove(MoveList& legalMoves, Bitboard pinned, uint8_t kingSquare,
        uint8_t start, uint8_t end) {
        if (!pinned.checkBit(start) ||
            PregeneratedMoves::getLine(start, kingSquare) ==
            PregeneratedMoves::getLine(start, end)) {
            legalMoves.add({ start, end });
        }
    }

    void tryAddPromotions(MoveList& legalMoves, Bitboard pinned, uint8_t kingSquare,
        uint8_t start, uint8_t end) {
        if (!pinned.checkBit(start) ||
            PregeneratedMoves::getLine(start, kingSquare) ==
            PregeneratedMoves::getLine(start, end)) {
            legalMoves.add({ start, end, PieceType::Bishop });
            legalMoves.add({ start, end, PieceType::Knight });
            legalMoves.add({ start, end, PieceType::Rook });
            legalMoves.add({ start, end, PieceType::Queen });
        }
    }

    void addPawnMoves(MoveList& legalMoves, const Position& position,
        Bitboard checkMask, Bitboard pinned, bool onlyCaptures) {
        const Bitboard pawns = position.getFriendlyPawns();
        const Bitboard pushMask = checkMask & ~position.getOccupied();
        const Bitboard captureMask = checkMask & position.getOpponentBitboard();

        Bitboard advanceOne;
        Bitboard advanceTwo;
        Bitboard captureLeft;
        Bitboard captureRight;
        Bitboard enPassantLeft;
        Bitboard enPassantRight;
        Bitboard promotionLeft;
        Bitboard promotionPush;
        Bitboard promotionRight;

        int offset;

        if (position.getTurn() == PieceColor::White) {
            const Bitboard north = pawns.north() & ~position.getOccupied();
            advanceOne = north & checkMask & ~Bitboard::mask8();
            promotionPush = north & checkMask & Bitboard::mask8();
            advanceTwo = (north & Bitboard::mask3()).north() & pushMask;

            const Bitboard northEast = pawns.northEast() & captureMask;
            captureRight = northEast & ~Bitboard::mask8();
            promotionRight = northEast & Bitboard::mask8();

            const Bitboard northWest = pawns.northWest() & captureMask;
            captureLeft = northWest & ~Bitboard::mask8();
            promotionLeft = northWest & Bitboard::mask8();

            const Bitboard enPassantTarget =
                (position.getEnPassantTargetBitboard().south() & checkMask).north();
            enPassantLeft = pawns.northWest() & enPassantTarget;
            enPassantRight = pawns.northEast() & enPassantTarget;

            offset = -8;
        }
        else {
            const Bitboard south = pawns.south() & ~position.getOccupied();
            advanceOne = south & checkMask & ~Bitboard::mask1();
            promotionPush = south & checkMask & Bitboard::mask1();
            advanceTwo = (south & Bitboard::mask6()).south() & pushMask;

            const Bitboard southEast = pawns.southEast() & captureMask;
            captureRight = southEast & ~Bitboard::mask1();
            promotionRight = southEast & Bitboard::mask1();

            const Bitboard southWest = pawns.southWest() & captureMask;
            captureLeft = southWest & ~Bitboard::mask1();
            promotionLeft = southWest & Bitboard::mask1();

            const Bitboard enPassantTarget =
                (position.getEnPassantTargetBitboard().north() & checkMask).south();
            enPassantLeft = pawns.southWest() & enPassantTarget;
            enPassantRight = pawns.southEast() & enPassantTarget;

            offset = 8;
        }

        const uint8_t kingSquare = position.getFriendlyKingSquare();
        if (!onlyCaptures) {
            while (advanceOne) {
                const uint8_t square = advanceOne.popLSB();
                tryAddPawnMove(legalMoves, pinned, kingSquare, square - offset, square);
            }

            while (advanceTwo) {
                const uint8_t square = advanceTwo.popLSB();
                tryAddPawnMove(legalMoves, pinned, kingSquare, square - 2 * offset,
                    square);
            }
            while (promotionPush) {
                const uint8_t square = promotionPush.popLSB();
                tryAddPromotions(legalMoves, pinned, kingSquare, square - offset, square);
            }
        }

        while (captureLeft) {
            const uint8_t square = captureLeft.popLSB();
            tryAddPawnMove(legalMoves, pinned, kingSquare, square - offset + 1, square);
        }

        while (captureRight) {
            const uint8_t square = captureRight.popLSB();
            tryAddPawnMove(legalMoves, pinned, kingSquare, square - offset - 1, square);
        }

        while (promotionLeft) {
            const uint8_t square = promotionLeft.popLSB();
            tryAddPromotions(legalMoves, pinned, kingSquare, square - offset + 1,
                square);
        }

        while (promotionRight) {
            const uint8_t square = promotionRight.popLSB();
            tryAddPromotions(legalMoves, pinned, kingSquare, square - offset - 1,
                square);
        }

        // WHY DOES EN PASSANT EXIST???? GOOD LUCK READING THIS
        if (position.canEnPassant()) {
            const Bitboard enPassantTarget =
                position.getTurn() == PieceColor::White
                ? position.getEnPassantTargetBitboard().south()
                : position.getEnPassantTargetBitboard().north();
            if (enPassantLeft) {
                const Bitboard kingRay = PregeneratedMoves::getRookMoves(
                    kingSquare, position.getOccupied() & ~enPassantTarget &
                    ~(enPassantTarget.east()));
                if (PregeneratedMoves::getLine(kingSquare,
                    position.getEnPassantTarget() - offset) !=
                    PregeneratedMoves::getLine(
                        kingSquare, position.getEnPassantTarget() - offset + 1) ||
                    !(kingRay & position.getOpponentOrthogonal())) {
                    tryAddPawnMove(legalMoves, pinned, kingSquare,
                        position.getEnPassantTarget() - offset + 1,
                        position.getEnPassantTarget());
                }
            }

            if (enPassantRight) {
                const Bitboard kingRay = PregeneratedMoves::getRookMoves(
                    kingSquare, position.getOccupied() & ~enPassantTarget &
                    ~(enPassantTarget.west()));
                if (PregeneratedMoves::getLine(kingSquare,
                    position.getEnPassantTarget() - offset) !=
                    PregeneratedMoves::getLine(
                        kingSquare, position.getEnPassantTarget() - offset - 1) ||
                    !(kingRay & position.getOpponentOrthogonal())) {
                    tryAddPawnMove(legalMoves, pinned, kingSquare,
                        position.getEnPassantTarget() - offset - 1,
                        position.getEnPassantTarget());
                }
            }
        }
    }

    void addCastlingMoves(MoveList& legalMoves, const Position& position,
        Bitboard dangerSquares) {
        const uint8_t kingSquare = position.getFriendlyKingSquare();

        const Bitboard kingsideCastleMask = position.getTurn() == PieceColor::White
            ? Bitboard{ 0b11ULL << 61 }
        : Bitboard{ 0b11ULL << 5 };
        if (position.canCastleKingside() &&
            !(position.getOccupied() & kingsideCastleMask) &&
            !(dangerSquares & kingsideCastleMask)) {
            legalMoves.add({ kingSquare, static_cast<uint8_t>(kingSquare + 2) });
        }

        const Bitboard queensideCastleFriendlyMask =
            position.getTurn() == PieceColor::White ? Bitboard{ 0b111ULL << 57 }
        : Bitboard{ 0b111ULL << 1 };
        const Bitboard queensideCastleDangerMask =
            position.getTurn() == PieceColor::White ? Bitboard{ 0b11ULL << 58 }
        : Bitboard{ 0b11ULL << 2 };
        if (position.canCastleQueenside() &&
            !(position.getOccupied() & queensideCastleFriendlyMask) &&
            !(dangerSquares & queensideCastleDangerMask)) {
            legalMoves.add({ kingSquare, static_cast<uint8_t>(kingSquare - 2) });
        }
    }

}  // namespace

bool MoveGenerator::generateLegal(const Position& position,
    MoveList& legalMoves, bool onlyCaptures) {
    legalMoves.clear();
    const Bitboard attackers = getAttackers(position);
    const Bitboard dangerSquares = getDangerSquares(position);

    addKingMoves(legalMoves, position, dangerSquares, onlyCaptures);

    const int numAttackers = attackers.numBits();
    if (numAttackers > 1) {
        return true;
    }

    Bitboard mask = Bitboard::full();
    if (numAttackers == 1) {
        const uint8_t square = attackers.getLSBIndex();
        // bishop, queen, or rook check means we can block or capture
        if ((position.getOpponentOrthogonal() | position.getOpponentDiagonal()) &
            attackers) {
            mask = PregeneratedMoves::getBetween(position.getFriendlyKingSquare(),
                square);
            // knight, pawn means we have to capture
        }
        else {
            mask = attackers;
        }
    }
    else if (!onlyCaptures) {
        addCastlingMoves(legalMoves, position, dangerSquares);
    }
    const Bitboard pinned = getPinned(position);

    addSlidingMoves(legalMoves, position, mask, pinned, onlyCaptures);
    addKnightMoves(legalMoves, position, mask, pinned, onlyCaptures);
    addPawnMoves(legalMoves, position, mask, pinned, onlyCaptures);
    return attackers;
}
