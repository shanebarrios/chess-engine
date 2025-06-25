#pragma once

#include <string_view>
#include <atomic>

class SharedState;

class IncomingChallengeHandler
{
public:
	explicit IncomingChallengeHandler(SharedState& state);

	void operator()();

	void run();
	void kill();
private:
	SharedState& m_sharedState;

	std::atomic_bool m_running{};

	void loop();
};

