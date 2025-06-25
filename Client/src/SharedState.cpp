#include "SharedState.hpp"

#include <algorithm>
#include "Logger.hpp"

void SharedState::enqueueChallenge(std::string_view gameId) {
	// someone told me unlocking before notifying is more EFFICIENT (+0.0000000000045% efficiency bonus)
	{
		std::lock_guard lock{ m_mutex }; 
		m_challenges.emplace_back(gameId);
	}
	m_queueCv.notify_one();
}

void SharedState::removeChallenge(std::string_view gameId) {
	std::lock_guard lock{ m_mutex };
	const auto it = std::find(m_challenges.begin(), m_challenges.end(), gameId);
	if (it != m_challenges.end()) {
		m_challenges.erase(it);
	}
}

void SharedState::awaitMainStreamStarted() {
	std::unique_lock lock{ m_mutex };
	m_mainStreamStartedCv.wait(lock, [this]() {
		return m_mainStreamStarted.load();
	});
}

std::string SharedState::awaitPopChallenge() {
	std::unique_lock lock{ m_mutex };
	m_queueCv.wait(lock, [this]() {
		return m_challenges.size() > 0;
	});
	const std::string ret = std::move(m_challenges.front());
	m_challenges.pop_front();
	return ret;
}

std::optional<GameStartEvent> SharedState::awaitGameStart(std::string_view gameId, int maxWaitSeconds) {
	std::unique_lock lock{ m_mutex };
	std::map<std::string, GameStartEvent>::iterator it;
	m_gameStartCv.wait_for(lock, std::chrono::seconds(maxWaitSeconds), [this, gameId, &it]() {
		it = m_gameStartEvents.find(std::string{ gameId });
		return it != m_gameStartEvents.end();
		});
	if (it == m_gameStartEvents.end())
	{
		LOG(gameId, " failed to start");
		return std::nullopt;
	}
	else
	{
		const GameStartEvent data = it->second;
		m_gameStartEvents.erase(it);
		return data;
	}
}

void SharedState::notifyMainStreamStarted() {
	std::call_once(m_mainStreamStartedFlag, [this]() {
		m_mainStreamStarted.store(true);
		m_mainStreamStartedCv.notify_all();
	});
}

void SharedState::notifyGameStart(const GameStartEvent& gameStart) {
	{
		std::lock_guard lock{ m_mutex };
		m_gameStartEvents[gameStart.id] = gameStart;
	}
	m_gameStartCv.notify_all();
}


