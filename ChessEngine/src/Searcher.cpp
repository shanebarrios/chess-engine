#include "Searcher.hpp"

#include <algorithm>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <thread>

#include "Evaluator.hpp"
#include "MoveGenerator.hpp"
#include "SortedMoves.hpp"
#include "Utils.hpp"

// TODO:
// Fix principal variation?
// Opening book

namespace {
    constexpr int maxDepth = 64;

    constexpr int posInfinity = 1000000000;
    constexpr int negInfinity = -posInfinity;

}

using namespace Chess;

int Searcher::quiescenceSearch(Position& position, int alpha, int beta) {
    int score = Evaluator::evaluate(position);
    if (score >= beta) {
        return beta;
    }
    if (score > alpha) {
        alpha = score;
    }

    SortedMoves legalMoves{ position, m_killerMoves, m_history, Move{},
                           0,        true,          nullptr };

    while (legalMoves.hasNext()) {
        Move move = legalMoves.getNext();
        position.makeMove(move);
        int score = -quiescenceSearch(position, -beta, -alpha);
        position.unmakeMove(move);
        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}

int Searcher::search(Position& position, int depth, int ply, int alpha,
    int beta, bool isPV) {
    if (m_timeUp) {
        return 0;
    }

    m_nodes++;

    if (position.hasRepeatedThreefold()) return 0;

    if (depth == 0) {
        return quiescenceSearch(position, alpha, beta);
    }

    if (auto hashedScore =
        m_transpositionTable.probeScore(position, depth, ply, alpha, beta)) {
        m_transpositions++;
        return *hashedScore;
    }

    Move hashedMove = m_transpositionTable.probeMove(position);

    bool inCheck;
    SortedMoves legalMoves{ position, m_killerMoves, m_history, hashedMove,
                           depth,    false,         &inCheck };

    if (legalMoves.size() == 0) {
        if (inCheck) {
            return -(posInfinity - ply);
        }
        else {
            return 0;
        }
    }

    TranspositionEntry::Flag flag = TranspositionEntry::Upper;
    Move choice;
    while (legalMoves.hasNext()) {
        Move move = legalMoves.getNext();

        position.makeMove(move);
        int score;
        if (isPV && flag == TranspositionEntry::Exact) {
            score = -search(position, depth - 1, ply + 1, -alpha - 1, -alpha, false);
            if (score > alpha) {
                score = -search(position, depth - 1, ply + 1, -beta, -alpha, true);
            }

        }
        else {
            score = -search(position, depth - 1, ply + 1, -beta, -alpha, isPV);
        }
        position.unmakeMove(move);

        if (m_timeUp) {
            return 0;
        }

        if (score > alpha) {
            alpha = score;
            choice = move;
            flag = TranspositionEntry::Exact;
        }
        if (score >= beta) {
            m_transpositionTable.tryStore(position, move, depth, beta,
                TranspositionEntry::Lower);
            if (!position.getPieceAt(move.target)) {
                m_killerMoves[depth][1] = m_killerMoves[depth][0];
                m_killerMoves[depth][0] = move;
                m_history[static_cast<uint8_t>(position.getTurn())][move.start]
                    [move.target] += depth * depth;
                if (m_history[static_cast<uint8_t>(position.getTurn())][move.start]
                    [move.target] >= k_killerScore) {
                    m_history[static_cast<uint8_t>(position.getTurn())][move.start]
                        [move.target] /= 2;
                }
            }

            return beta;
        }
    }
    m_transpositionTable.tryStore(position, choice, depth, alpha, flag);
    return alpha;
}

std::pair<Move, int> Searcher::rootSearch(Position& position, int depth) {
    int alpha = negInfinity - maxDepth;
    int beta = posInfinity + maxDepth;
    Move choice;

    Move hashedMove = m_transpositionTable.probeMove(position);
    SortedMoves legalMoves{ position, m_killerMoves, m_history, hashedMove,
                           depth,    false,         nullptr };

    while (legalMoves.hasNext()) {
        Move move = legalMoves.getNext();

        position.makeMove(move);
        int score;
        if (alpha == negInfinity - maxDepth) {
            score = -search(position, depth - 1, 0, -beta, -alpha, true);
        }
        else {
            score = -search(position, depth - 1, 0, -alpha - 1, -alpha, false);
            if (score > alpha) {
                score = -search(position, depth - 1, 0, -beta, -alpha, true);
            }
        }
        position.unmakeMove(move);

        if (m_timeUp) {
            return {};
        }

        if (score > alpha) {
            alpha = score;
            choice = move;
            m_transpositionTable.tryStore(position, choice, depth, alpha,
                TranspositionEntry::Lower);
        }
    }

    m_transpositionTable.tryStore(position, choice, depth, alpha,
        TranspositionEntry::Exact);

    return { choice, alpha };
}

Move Searcher::getMove(const Position& position, int thinkMilliseconds) {
    Move choice;
    int eval;
    std::thread timeUpThread{ [this, thinkMilliseconds]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(thinkMilliseconds));
      this->m_timeUp = true;
    } };
    int depth = 1;

    Position clone{ position };
    for (; depth < 64; depth++) {
        auto [move, score] = rootSearch(clone, depth);
        if (m_timeUp || move == Move{}) {
            break;
        }
        choice = move;
        eval = score;
    }

    std::memset(m_killerMoves.data(), 0, sizeof(m_killerMoves));
    std::memset(m_history.data(), 0, sizeof(m_history));

    m_nodes = 0;
    m_transpositions = 0;

    timeUpThread.join();
    m_timeUp = false;
    return choice;
}