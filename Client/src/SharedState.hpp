#pragma once

#include <string>
#include <string_view>
#include <list>
#include <format>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <map>
#include <nlohmann/json.hpp>
#include <Chess.hpp>

using json = nlohmann::json;

struct GameStartEvent
{
	std::string startFen;
	Chess::PieceColor color;
	std::string id;
	int timePerSide;
};

class SharedState {
public:
	SharedState() = default;

	void enqueueChallenge(std::string_view challengeId);

	void removeChallenge(std::string_view challengeId);

	void awaitMainStreamStarted();

	std::string awaitPopChallenge();

	std::optional<GameStartEvent> awaitGameStart(std::string_view id, int maxWaitSeconds = 10);

	void notifyMainStreamStarted();

	void notifyGameStart(const GameStartEvent& gameStart);
private:
	std::list<std::string> m_challenges {};
	// virgin std::unordered_map vs chad std::map
	std::map<std::string, GameStartEvent> m_gameStartEvents{};

	std::mutex m_mutex{};
	std::condition_variable m_queueCv{};
	std::condition_variable m_gameStartCv{};
	std::condition_variable m_mainStreamStartedCv{};

	std::once_flag m_mainStreamStartedFlag{};

	std::atomic_bool m_mainStreamStarted{};
};