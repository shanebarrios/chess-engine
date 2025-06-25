#include "SortedMoves.hpp"

#include <algorithm>

#include "Evaluator.hpp"
#include "MoveGenerator.hpp"

using namespace Chess;

static int scoreMove(const Position& position, const Array2D<Move, 64, 2>& killerMoves,
    const Array3D<int, 2, 64, 64>& history, Move hashedMove, int depth,
    Move move) {
    if (move == hashedMove) {
        return INT32_MAX;
    }
    Piece toMove = position.getPieceAt(move.start);
    Piece captured = position.getPieceAt(move.target);
    if (move.promotion != PieceType::Null) {
        return k_killerScore + 1000 + Evaluator::evaluatePiece(move.promotion) - 100;
    }
    else if (captured.type != PieceType::Null) {
        return k_killerScore + 1000 + Evaluator::evaluatePiece(captured.type) -
            Evaluator::evaluatePiece(toMove.type);
    }
    else if (killerMoves[depth][0] == move || killerMoves[depth][1] == move) {
        return k_killerScore;
    }
    else {
        return history[static_cast<uint8_t>(position.getTurn())][move.start]
            [move.target];
    }
}

SortedMoves::SortedMoves(const Position& position, const Array2D<Move, 64, 2>& killerMoves,
    const Array3D<int, 2, 64, 64>& history, Move hashedMove, int depth,
    bool onlyCaptures, bool* checkStatus) {
    bool inCheck =
        MoveGenerator::generateLegal(position, m_moveList, onlyCaptures);
    if (checkStatus != nullptr) {
        *checkStatus = inCheck;
    }
    for (int i = 0; i < m_moveList.size(); i++) {
        m_scores[i] = scoreMove(position, killerMoves, history, hashedMove, depth,
            m_moveList[i]);
    }
}

bool SortedMoves::hasNext() const { return m_index < m_moveList.size(); }

Move SortedMoves::getNext() {
    int maxI = m_index;
    int max = m_scores[m_index];
    for (int j = m_index + 1; j < m_moveList.size(); j++) {
        int score = m_scores[j];
        if (score > max) {
            max = score;
            maxI = j;
        }
    }
    std::swap(m_moveList[m_index], m_moveList[maxI]);
    m_scores[maxI] = m_scores[m_index];
    return m_moveList[m_index++];
}

uint8_t SortedMoves::size() const { return m_moveList.size(); }