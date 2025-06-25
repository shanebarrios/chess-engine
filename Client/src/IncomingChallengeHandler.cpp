#include "IncomingChallengeHandler.hpp"

#include <format>
#include <chrono>

#include "SharedState.hpp"
#include "Curl.hpp"
#include "Logger.hpp"
#include "GameHandler.hpp"
#include "Authorization.hpp"
 
static constexpr std::string_view k_acceptChallengeURL = "https://lichess.org/api/challenge/{}/accept";

IncomingChallengeHandler::IncomingChallengeHandler(SharedState& state) : m_sharedState{ state }{}

void IncomingChallengeHandler::operator()() {
	run();
}

void IncomingChallengeHandler::run() {
	m_running.store(true);
	m_sharedState.awaitMainStreamStarted();
	while (m_running.load()) {
		loop();
	}
}

void IncomingChallengeHandler::loop()
{
	const std::string_view header = Authorization::instance().getAuthorizationHeader();
	std::string id;
	LOG("Waiting for challenge");
	while (true) {
		id = m_sharedState.awaitPopChallenge();
		const std::string url = std::format(k_acceptChallengeURL, id);
		const Curl curl = Curl::post(url, { header }, {});
		const CurlResponse response = curl.perform();

		if (response.responseCode != 200) {
			LOG("Failed to accept challenge, retrying");
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
		else {
			break;
		}
	}
	std::optional<GameStartEvent> gameData = m_sharedState.awaitGameStart(id, 5);
	if (gameData)
	{
		GameHandler gameHandler{ *gameData };
		gameHandler.run();
	}
}

void IncomingChallengeHandler::kill() {
	m_running.store(false);
}