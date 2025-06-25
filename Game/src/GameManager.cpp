#include "GameManager.hpp"

#include <algorithm>
#include <vector>

#include "InputHandler.hpp"

#define BIND_ON_MOVE_FUNC [this](const Player& player, Chess::Move move){this->onMove(player, move); }

GameManager::GameManager(GameType gameType) : m_gameType{ gameType }, m_callbackHandler{ BIND_ON_MOVE_FUNC } {
    switch (gameType) {
    case GameType::ComputerComputer:
        m_whitePlayer = std::make_unique<AIPlayer>(Chess::PieceColor::White, m_callbackHandler);
        m_blackPlayer = std::make_unique<AIPlayer>(Chess::PieceColor::Black, m_callbackHandler);
        break;
    case GameType::PlayerComputer:
        m_whitePlayer = std::make_unique<HumanPlayer>(Chess::PieceColor::White, m_callbackHandler);
        m_blackPlayer = std::make_unique<AIPlayer>(Chess::PieceColor::Black, m_callbackHandler);
        break;
    case GameType::ComputerPlayer:
        m_whitePlayer = std::make_unique<AIPlayer>(Chess::PieceColor::White, m_callbackHandler);
        m_blackPlayer = std::make_unique<HumanPlayer>(Chess::PieceColor::Black, m_callbackHandler);
        break;
    }
    m_position = Chess::Position::defaultPosition();
    m_whitePlayer->updatePosition(m_position);
    m_blackPlayer->updatePosition(m_position);
    Chess::MoveGenerator::generateLegal(m_position, m_legalMoves);

    if (m_position.getTurn() == Chess::PieceColor::White) {
        m_whitePlayer->queryMove();
    }
    else {
        m_blackPlayer->queryMove();
    }
}

const Chess::Position& GameManager::getPosition() const { return m_position; }

const HumanPlayer* GameManager::getHumanPlayer() const {
    if (m_gameType == GameType::PlayerComputer) {
        return dynamic_cast<const HumanPlayer*>(m_whitePlayer.get());
    }
    else if (m_gameType == GameType::ComputerPlayer) {
        return dynamic_cast<const HumanPlayer*>(m_blackPlayer.get());
    }
    else {
        return nullptr;
    }
}

const Chess::MoveList& GameManager::getLegalMoves() const { return m_legalMoves; }

Chess::Piece GameManager::getPiece(uint8_t square) const {
    return m_position.getPieceAt(square);
}

void GameManager::onMove(const Player& player, Chess::Move move) {
    if (player.getColor() != m_position.getTurn()) {
        return;
    }
    // auto queen promotion for user
    if (m_position.getPieceAt(move.start).type == Chess::PieceType::Pawn &&
        (move.target / 8 == 0 || move.target / 8 == 7) &&
        move.promotion == Chess::PieceType::Null) {
        move.promotion = Chess::PieceType::Queen;
    }
    for (Chess::Move m : m_legalMoves) {
        if (move == m) {
            m_position.makeMove(move);
            Chess::MoveGenerator::generateLegal(m_position, m_legalMoves);
            m_whitePlayer->updatePosition(m_position);
            m_blackPlayer->updatePosition(m_position);
            if (m_legalMoves.size() > 0) {
                m_position.getTurn() == Chess::PieceColor::White ? m_whitePlayer->queryMove()
                    : m_blackPlayer->queryMove();
            }
            // quick hack to stop blocking of rendering when AI moves, should definitely change this
            InputHandler::instance().nullPush();
            break;
        }
    }
}
