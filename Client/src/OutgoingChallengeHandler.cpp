#include "OutgoingChallengeHandler.hpp"

#include "SharedState.hpp"
#include "Authorization.hpp"
#include "Curl.hpp"
#include "Logger.hpp"
#include "GameHandler.hpp"
#include <chrono>

static constexpr std::string_view k_challengeAIUrl = "https://lichess.org/api/challenge/ai";

OutgoingChallengeHandler::OutgoingChallengeHandler(SharedState& state) :
	m_sharedState {state} {}

void OutgoingChallengeHandler::operator()() {
	run();
}

void OutgoingChallengeHandler::run() {
	m_running.store(true);
	m_sharedState.awaitMainStreamStarted();
	while (m_running.load()) {
		loop();
	}
}

void OutgoingChallengeHandler::loop() {
	LOG("Sending AI challenge");
	
	const std::optional<std::string> gameId = sendAIChallenge();

	if (gameId) {
		const std::optional<GameStartEvent> gameData = m_sharedState.awaitGameStart(*gameId, 3);
		if (gameData) {
			GameHandler gameHandler{ *gameData };
			gameHandler.run();
		}
	}
	else {
		LOG("Failed to send AI challenge, trying again in 60 seconds");
		std::this_thread::sleep_for(std::chrono::seconds(60));
	}
}

std::optional<std::string> OutgoingChallengeHandler::sendAIChallenge() {
	const std::string_view authorizationHeader = Authorization::instance().getAuthorizationHeader();
	constexpr std::string_view contentTypeHeader = "Content-Type: application/x-www-form-urlencoded";

	std::string id;
	bool fail = false;
	Curl curl = Curl::post(k_challengeAIUrl, { authorizationHeader, contentTypeHeader }, { {"level", "6"} });
	curl.setCallback([&id, &fail](std::string_view s) {
		json data;
		try {
			data = json::parse(s);
		}
		catch (...) {
			fail = true;
			return CURL_BREAK;
		}

		if (!data.contains("id")) {
			fail = true;
			return CURL_BREAK;
		}

		id = data["id"];
		return CURL_BREAK;
		});
	const CurlResponse response = curl.perform();
	if (fail || response.responseCode != 201) {
		return std::nullopt;
	}
	else {
		return id;
	}
}

void OutgoingChallengeHandler::kill() {
	m_running.store(false);
}