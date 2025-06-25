#pragma once

#include "SharedState.hpp"
#include <string_view>
#include <Chess.hpp>

class GameHandler
{
public:
	explicit GameHandler(const GameStartEvent& gameStart);

	void operator()();
	void run();
private:
	Chess::Searcher m_searcher{};
	Chess::Position m_position;
	Chess::PieceColor m_color;

	std::string m_id;
	int m_timePerSide;

	bool sendMove();
};