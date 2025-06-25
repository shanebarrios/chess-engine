#pragma once
#include <cstdint>
#include <stack>
#include <string_view>

#include "Bitboard.hpp"
#include "Move.hpp"
#include "Piece.hpp"
#include "Zobrist.hpp"
#include "DataStructures.hpp"

namespace Chess {
    class Position {
    public:
        static constexpr uint8_t invalidSquare = 0xFF;
        static Position fromFen(std::string_view fen);
        static Position defaultPosition();

        inline Bitboard getBitboard(PieceType piece, PieceColor color) const {
            return m_bitboards[static_cast<uint8_t>(piece)]
                [static_cast<uint8_t>(color)];
        }
        inline Bitboard getFriendlyBitboard() const {
            return m_colorBitboards[static_cast<uint8_t>(m_turn)];
        }
        inline Bitboard getOpponentBitboard() const {
            return m_colorBitboards[static_cast<uint8_t>(getOppositeTurn())];
        }

        inline Bitboard getFriendlyPawns() const {
            return getBitboard(PieceType::Pawn, m_turn);
        }

        inline Bitboard getOpponentPawns() const {
            return getBitboard(PieceType::Pawn, getOppositeTurn());
        }

        inline Bitboard getFriendlyBishops() const {
            return getBitboard(PieceType::Bishop, m_turn);
        }

        inline Bitboard getOpponentBishops() const {
            return getBitboard(PieceType::Bishop, getOppositeTurn());
        }

        inline Bitboard getFriendlyKnights() const {
            return getBitboard(PieceType::Knight, m_turn);
        }

        inline Bitboard getOpponentKnights() const {
            return getBitboard(PieceType::Knight, getOppositeTurn());
        }

        inline Bitboard getFriendlyRooks() const {
            return getBitboard(PieceType::Rook, m_turn);
        }

        inline Bitboard getOpponentRooks() const {
            return getBitboard(PieceType::Rook, getOppositeTurn());
        }

        inline Bitboard getFriendlyQueens() const {
            return getBitboard(PieceType::Queen, m_turn);
        }

        inline Bitboard getOpponentQueens() const {
            return getBitboard(PieceType::Queen, getOppositeTurn());
        }

        inline Bitboard getFriendlyOrthogonal() const {
            return getBitboard(PieceType::Rook, m_turn) |
                getBitboard(PieceType::Queen, m_turn);
        }

        inline Bitboard getOpponentOrthogonal() const {
            return getBitboard(PieceType::Rook, getOppositeTurn()) |
                getBitboard(PieceType::Queen, getOppositeTurn());
        }

        inline Bitboard getFriendlyDiagonal() const {
            return getBitboard(PieceType::Bishop, m_turn) |
                getBitboard(PieceType::Queen, m_turn);
        }

        inline Bitboard getOpponentDiagonal() const {
            return getBitboard(PieceType::Bishop, getOppositeTurn()) |
                getBitboard(PieceType::Queen, getOppositeTurn());
        }

        inline Bitboard getOccupied() const {
            return m_colorBitboards[0] | m_colorBitboards[1];
        };

        inline uint8_t getFriendlyKingSquare() const {
            return getBitboard(PieceType::King, m_turn).getLSBIndex();
        }

        inline uint8_t getOpponentKingSquare() const {
            return getBitboard(PieceType::King, getOppositeTurn()).getLSBIndex();
        }

        inline PieceColor getTurn() const { return m_turn; };
        inline PieceColor getOppositeTurn() const {
            return static_cast<PieceColor>(static_cast<uint8_t>(m_turn) ^ 1);
        };

        inline Bitboard getEnPassantTargetBitboard() const {
            return m_state.enPassantTarget == invalidSquare
                ? Bitboard::empty()
                : Bitboard::fromSquare(m_state.enPassantTarget);
        }

        inline uint8_t getEnPassantTarget() const { return m_state.enPassantTarget; }

        inline bool canEnPassant() const {
            return m_state.enPassantTarget != invalidSquare;
        }

        inline uint8_t getCastlingFlags() const { return m_state.castlingFlags; }

        inline Zobrist getZobrist() const { return m_state.hash; }

        bool canCastleKingside() const;
        bool canCastleQueenside() const;
        inline Piece getPieceAt(uint8_t square) const { return m_pieces[square]; }

        void makeMove(Move move);
        void unmakeMove(Move move);

        bool hasRepeatedThreefold() const;

    private:
        enum CastlingFlag : uint8_t {
            WhiteCastleKingside = 0b1,
            WhiteCastleQueenside = 0b10,
            BlackCastleKingside = 0b100,
            BlackCastleQueenside = 0b1000
        };

        struct State {
            uint8_t castlingFlags{};
            uint8_t enPassantTarget{ invalidSquare };
            uint8_t halfMoveClock{};
            Zobrist hash{};
        };

        struct UndoState {
            State state{};
            Piece captured{};
        };

        Array2D<Bitboard, 6, 2> m_bitboards{};
        Array<Bitboard, 2> m_colorBitboards{};

        PieceColor m_turn{ PieceColor::White };

        Array<Piece, 64> m_pieces{};

        State m_state{};

        int m_ply{ 0 };

        FixedStack<UndoState, 1024> m_positionHistory{};

        void addPiece(PieceType type, PieceColor color, uint8_t square);
        void addPieceAndUpdateZobrist(PieceType type, PieceColor color, uint8_t square);
        void removePiece(PieceType type, PieceColor color, uint8_t square);
        void removePieceAndUpdateZobrist(PieceType type, PieceColor color, uint8_t square);
        void movePiece(PieceType type, PieceColor color, uint8_t src, uint8_t dst);
        void movePieceAndUpdateZobrist(PieceType type, PieceColor color, uint8_t src, uint8_t dst);

        void setCastleKingside(bool flag);
        void setCastleQueenside(bool flag);
    };
}