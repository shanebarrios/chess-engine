#include "Transposition.hpp"

#include "Position.hpp"

using namespace Chess;

void TranspositionTable::tryStore(const Position& position, Move move,
    int depth, int score,
    TranspositionEntry::Flag flag) {
    Zobrist key = position.getZobrist();
    TranspositionEntry* entry =
        &m_table[static_cast<uint64_t>(key) & (m_table.size() - 1)];
    if (entry->key == key && entry->depth > depth) {
        return;
    }
    *entry = { key, move, static_cast<uint8_t>(depth), score, flag };
}

std::optional<int> TranspositionTable::probeScore(const Position& position,
    int depth, int ply, int alpha,
    int beta) {
    Zobrist key = position.getZobrist();
    TranspositionEntry* entry =
        &m_table[static_cast<uint64_t>(key) & (m_table.size() - 1)];
    if (entry->key == key) {
        if (entry->depth >= depth) {
            int score = entry->score;
            // adjust score for mate
            if (score > k_infinity - 256) {
                score -= ply;
            }
            if (score < -k_infinity + 256) {
                score += ply;
            }
            if (entry->flag == TranspositionEntry::Exact) {
                return score;
            }
            if (entry->flag == TranspositionEntry::Upper && alpha >= entry->score) {
                return score;
            }
            if (entry->flag == TranspositionEntry::Lower && beta <= entry->score) {
                return score;
            }
        }
    }
    return std::nullopt;
}

Move TranspositionTable::probeMove(const Position& position) {
    Zobrist key = position.getZobrist();
    TranspositionEntry* entry =
        &m_table[static_cast<uint64_t>(key) & (m_table.size() - 1)];
    if (entry->key == key) {
        return entry->move;
    }
    return Move{};
}