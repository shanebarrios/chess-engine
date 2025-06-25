#pragma once

#include <string_view>
#include <atomic>
#include <optional>
#include <string>

class SharedState;

class OutgoingChallengeHandler
{
public:
	explicit OutgoingChallengeHandler(SharedState& state);

	void operator()();

	void run();
	void kill();
private:
	SharedState& m_sharedState;
	std::atomic_bool m_running{};

	void loop();
	std::optional<std::string> sendAIChallenge();
};