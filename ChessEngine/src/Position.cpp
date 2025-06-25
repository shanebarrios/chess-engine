#include "Position.hpp"

#include <cctype>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Utils.hpp"
#include "SquareAliases.hpp"

using namespace Chess;

Position Position::fromFen(std::string_view fen) {
    static const std::unordered_map<char, PieceType> pieceMapping{
        {'p', PieceType::Pawn},   {'b', PieceType::Bishop},
        {'n', PieceType::Knight}, {'r', PieceType::Rook},
        {'q', PieceType::Queen},  {'k', PieceType::King} };

    static const std::unordered_map<char, CastlingFlag> castlingMapping{
        {'K', CastlingFlag::WhiteCastleKingside},
        {'Q', CastlingFlag::WhiteCastleQueenside},
        {'k', CastlingFlag::BlackCastleKingside},
        {'q', CastlingFlag::BlackCastleQueenside} };

    if (fen == "startpos") {
        return Position::defaultPosition();
    }

    Position position{};
    const std::vector splitFen{ Utils::split(fen) };
    if (splitFen.size() != 6) {
        throw std::invalid_argument{ "Invalid FEN piece placement" };
    }
    uint8_t row = 0;
    uint8_t col = 0;
    for (char c : splitFen[0]) {
        if (c == '/' && col == 8) {
            row++;
            col = 0;
        }
        else if (row >= 8 || col >= 8) {
            throw std::invalid_argument{ "Invalid FEN piece placement" };
        }
        else if (auto it = pieceMapping.find(tolower(c));
            it != pieceMapping.end()) {
            PieceType type{ it->second };
            PieceColor color{ isupper(c) ? PieceColor::White : PieceColor::Black };
            position.addPiece(type, color, row * 8 + col);
            col++;
        }
        else if (isdigit(c)) {
            col += c - '0';
        }
        else {
            throw std::invalid_argument{ "Invalid FEN piece placement" };
        }
    }

    if (splitFen[1].size() != 1) {
        throw std::invalid_argument{ "Invalid FEN active color" };
    }
    else if (splitFen[1][0] == 'w') {
        position.m_turn = PieceColor::White;
    }
    else if (splitFen[1][0] == 'b') {
        position.m_turn = PieceColor::Black;
    }
    else {
        throw std::invalid_argument{ "Invalid FEN active color" };
    }

    if (splitFen[2].size() > 1 || splitFen[2][0] != '-') {
        for (char c : splitFen[2]) {
            if (auto it = castlingMapping.find(c); it != castlingMapping.end()) {
                position.m_state.castlingFlags |= it->second;
            }
            else {
                throw std::invalid_argument{ "Invalid FEN castling rights" };
            }
        }
    }

    if (splitFen[3].size() == 2) {
        const int file = splitFen[3][0] - 'a';
        const int rank = splitFen[3][1] - '0';
        if (rank < 0 || rank >= 8 || file < 0 || file >= 8) {
            throw std::invalid_argument{ "Invalid FEN en passant target" };
        }
        position.m_state.enPassantTarget = 8 * (8 - rank) + file;
    }
    else if (splitFen[3].size() != 1 || splitFen[3][0] != '-') {
        throw std::invalid_argument{ "Invalid FEN en passant target" };
    }
    position.m_state.hash = Zobrist::fromPosition(
        position);  // the hash was already partially computed as a side
    // effect of this function but who cares
    return position;
}

Position Position::defaultPosition() {
    return Position::fromFen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Position::setCastleKingside(bool flag) {
    const uint8_t mask =
        (m_turn == PieceColor::White) ? WhiteCastleKingside : BlackCastleKingside;
    if (flag) {
        m_state.castlingFlags |= mask;
    }
    else {
        m_state.castlingFlags &= ~mask;
    }
}

void Position::setCastleQueenside(bool flag) {
    const uint8_t mask = (m_turn == PieceColor::White) ? WhiteCastleQueenside
        : BlackCastleQueenside;
    if (flag) {
        m_state.castlingFlags |= mask;
    }
    else {
        m_state.castlingFlags &= ~mask;
    }
}

bool Position::canCastleKingside() const {
    const uint8_t mask =
        (m_turn == PieceColor::White) ? WhiteCastleKingside : BlackCastleKingside;
    return m_state.castlingFlags & mask;
}

bool Position::canCastleQueenside() const {
    const uint8_t mask = (m_turn == PieceColor::White) ? WhiteCastleQueenside
        : BlackCastleQueenside;
    return m_state.castlingFlags & mask;
}

void Position::addPiece(PieceType piece, PieceColor color, uint8_t square) {
    m_bitboards[static_cast<uint8_t>(piece)][static_cast<uint8_t>(color)].setBit(
        square);
    m_colorBitboards[static_cast<uint8_t>(color)].setBit(square);
    m_pieces[square] = Piece{ piece, color };
}

void Position::addPieceAndUpdateZobrist(PieceType piece, PieceColor color, uint8_t square) {
    addPiece(piece, color, square);
    m_state.hash.togglePiece(piece, color, square);
}

void Position::removePiece(PieceType piece, PieceColor color, uint8_t square) {
    m_bitboards[static_cast<uint8_t>(piece)][static_cast<uint8_t>(color)]
        .clearBit(square);
    m_colorBitboards[static_cast<uint8_t>(color)].clearBit(square);
    m_pieces[square] = Piece{};
}

void Position::removePieceAndUpdateZobrist(PieceType piece, PieceColor color, uint8_t square) {
    removePiece(piece, color, square);
    m_state.hash.togglePiece(piece, color, square);
}

void Position::movePiece(PieceType piece, PieceColor color, uint8_t src,
    uint8_t dst) {
    Bitboard& bb = m_bitboards[static_cast<uint8_t>(piece)][static_cast<uint8_t>(color)];
    Bitboard& colorBB = m_colorBitboards[static_cast<uint8_t>(color)];
    bb.clearBit(src);
    bb.setBit(dst);
    colorBB.clearBit(src);
    colorBB.setBit(dst);
    m_pieces[src] = Piece{};
    m_pieces[dst] = Piece{ piece, color };
}

void Position::movePieceAndUpdateZobrist(PieceType piece, PieceColor color, uint8_t src, uint8_t dst) {
    Bitboard& bb = m_bitboards[static_cast<uint8_t>(piece)][static_cast<uint8_t>(color)];
    Bitboard& colorBB = m_colorBitboards[static_cast<uint8_t>(color)];
    bb.clearBit(src);
    bb.setBit(dst);
    colorBB.clearBit(src);
    colorBB.setBit(dst);
    m_pieces[src] = Piece{};
    m_pieces[dst] = Piece{ piece, color };
    m_state.hash.togglePiece(piece, color, src);
    m_state.hash.togglePiece(piece, color, dst);
}

void Position::makeMove(Move move) {
    const Piece toMove = m_pieces[move.start];
    const Piece captured = m_pieces[move.target];
    m_positionHistory.push({.state = m_state, .captured = captured});

    m_ply++;
    m_state.halfMoveClock++;
    
    m_state.hash.toggleCastlingFlags(m_state.castlingFlags);

    if (captured) {
        removePieceAndUpdateZobrist(captured.type, captured.color, move.target);
    }
    if (captured || toMove.type == PieceType::Pawn) {
        m_state.halfMoveClock = 0;
    }

    movePieceAndUpdateZobrist(toMove.type, toMove.color, move.start, move.target);

    // kingside castle
    if (toMove.type == PieceType::King) {
        if (move.target - move.start == 2) {
            movePieceAndUpdateZobrist(PieceType::Rook, m_turn, move.start + 3, move.start + 1);
        }
        if (move.start - move.target == 2) {
            movePieceAndUpdateZobrist(PieceType::Rook, m_turn, move.start - 4, move.start - 1);
        }
        setCastleKingside(false);
        setCastleQueenside(false);
    }

    if (toMove.type == PieceType::Pawn && move.target == m_state.enPassantTarget) {
        removePieceAndUpdateZobrist(PieceType::Pawn, getOppositeTurn(),
            m_state.enPassantTarget + (m_turn == PieceColor::White ? 8 : -8));
    }

    if (m_state.enPassantTarget != invalidSquare) {
        m_state.hash.toggleEnPassantFile(m_state.enPassantTarget % 8);
    }
    // set en passant square
    if (toMove.type == PieceType::Pawn && abs(move.start - move.target) == 16) {
        m_state.hash.toggleEnPassantFile(move.start % 8);
        m_state.enPassantTarget = move.start + (m_turn == PieceColor::White ? -8 : 8);
    }
    else {
        m_state.enPassantTarget = invalidSquare;
    }

    // handle promotion
    if (move.promotion != PieceType::Null) {
        removePieceAndUpdateZobrist(PieceType::Pawn, m_turn, move.target);
        addPieceAndUpdateZobrist(move.promotion, m_turn, move.target);
    }

    // update castling flags for moving or capturing rook
    if (move.start == Square::h1 || move.target == Square::h1) {
        m_state.castlingFlags &= ~WhiteCastleKingside;
    }
    if (move.start == Square::a1 || move.target == Square::a1) {
        m_state.castlingFlags &= ~WhiteCastleQueenside;
    }
    if (move.start == Square::h8 || move.target == Square::h8) {
        m_state.castlingFlags &= ~BlackCastleKingside;
    }
    if (move.start == Square::a8 || move.target == Square::a8) {
        m_state.castlingFlags &= ~BlackCastleQueenside;
    }
    m_state.hash.toggleCastlingFlags(m_state.castlingFlags);
    m_state.hash.toggleSide();
    m_turn = getOppositeTurn();
}

void Position::unmakeMove(Move move) {
    const UndoState undoState = m_positionHistory.top();
    const Piece captured = undoState.captured;
    m_state = undoState.state;
    m_positionHistory.pop();
    m_turn = getOppositeTurn();
    m_ply--;

    const Piece moved = m_pieces[move.target];
    const PieceType movedType = move.promotion == PieceType::Null ? moved.type : PieceType::Pawn;

    addPiece(movedType, moved.color, move.start);
    removePiece(moved.type, moved.color, move.target);
    if (captured) addPiece(captured.type, captured.color, move.target);
    
    if (move.target == m_state.enPassantTarget && movedType == PieceType::Pawn) {
        const int offset = moved.color == PieceColor::White ? 8 : -8;
        addPiece(PieceType::Pawn, getOppositeTurn(), m_state.enPassantTarget + offset);
    }

    if (moved.type == PieceType::King) {
        // Undo kingside castle
        if (move.target - move.start == 2) {
            movePiece(PieceType::Rook, m_turn, move.start + 1, move.start + 3);
        }
        // Undo queenside castle
        else if (move.start - move.target == 2) {
            movePiece(PieceType::Rook, m_turn, move.start - 1, move.start - 4);
        }
    }
}

bool Position::hasRepeatedThreefold() const {
    int startPly = m_ply - m_state.halfMoveClock - 1;
    if (startPly < 0) startPly = 0;
    int repetitionCount = 1;
    for (int i = m_positionHistory.size() - 2; i >= startPly; i -= 2) {
        if (m_positionHistory[i].state.hash == m_state.hash) {
            repetitionCount++;
        }
    }
    return repetitionCount >= 3;
}
