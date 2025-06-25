#pragma once

#include <vector>
#include <memory>

#include "Player.hpp"
#include <Chess.hpp>

enum class GameType { PlayerComputer, ComputerPlayer, ComputerComputer };

class GameManager {
public:
	explicit GameManager(GameType gameType);

	void onMove(const Player& player, Chess::Move move);

	const Chess::Position& getPosition() const;
	const HumanPlayer* getHumanPlayer() const;
	const Chess::MoveList& getLegalMoves() const;
	Chess::Piece getPiece(uint8_t square) const;

private:
	GameType m_gameType{};

	std::unique_ptr<Player> m_whitePlayer{};
	std::unique_ptr<Player> m_blackPlayer{};

	Chess::MoveList m_legalMoves{};
	Chess::Position m_position{};

	OnMoveCallback m_callbackHandler{};
};