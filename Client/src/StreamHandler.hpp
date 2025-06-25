#pragma once

#include <string_view>
#include <nlohmann/json.hpp>
#include <thread>
#include <atomic>
#include <array>
#include <memory>
#include "IncomingChallengeHandler.hpp"
#include "OutgoingChallengeHandler.hpp"
#include "SharedState.hpp"

using json = nlohmann::json;

static constexpr size_t k_numIncomingThreads = 4;
static constexpr size_t k_numOutgoingThreads = 1;

class StreamHandler
{
public:
	StreamHandler();

	void operator()();

	void run();
	void kill();
private:
	SharedState m_sharedState{};
	std::atomic_bool m_running{};

	std::array<std::unique_ptr<IncomingChallengeHandler>, k_numIncomingThreads> m_incomingChallengeHandlers{};
	std::array<std::unique_ptr<OutgoingChallengeHandler>, k_numOutgoingThreads> m_outgoingChallengeHandlers{};
	std::array<std::thread, k_numIncomingThreads> m_incomingChallengeThreads{};
	std::array<std::thread, k_numOutgoingThreads> m_outgoingChallengeThreads{};

	void listen();
	bool handleLine(std::string_view message);
};

