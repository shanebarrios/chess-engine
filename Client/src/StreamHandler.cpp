#include "StreamHandler.hpp"
#include "Curl.hpp"
#include "IncomingChallengeHandler.hpp"
#include "Logger.hpp"
#include "Authorization.hpp"
#include <iostream>
#include <utility>

static constexpr std::string_view k_streamEventsURL = "https://lichess.org/api/stream/event";
static constexpr std::string_view k_streamGameURL = "https://lichess.org/api/bot/game/stream/{}";
static constexpr std::string_view k_declineChallengeURL = "https://lichess.org/api/challenge/{}/decline";

// A good programmer would move these to a config file (what am I doing?????)
static constexpr std::array<std::string_view, 1> k_supportedVariants = {
	"standard"
};
static constexpr std::array<std::string_view, 3> k_supportedTimeControls = {
	"rapid", "blitz", "bullet"
};

template <typename T, typename U, size_t N>
static bool contains(const std::array<T, N>& arr, const U& val) {
	return std::find(arr.begin(), arr.end(), T{ val }) != arr.end();
}

template <typename T, size_t N>
static bool contains(const std::array<T, N>& arr, const T& val) {
	return std::find(arr.begin(), arr.end(), val) != arr.end();
}

StreamHandler::StreamHandler() {
	for (size_t i = 0; i < k_numIncomingThreads; i++) {
		m_incomingChallengeHandlers[i] = std::make_unique<IncomingChallengeHandler>(m_sharedState);
	}
	for (size_t i = 0; i < k_numOutgoingThreads; i++) {
		m_outgoingChallengeHandlers[i] = std::make_unique<OutgoingChallengeHandler>(m_sharedState);
	}
}

void StreamHandler::operator()() {
	run();
}

void StreamHandler::run() {
	m_running.store(true);
	for (size_t i = 0; i < k_numIncomingThreads; i++) {
		m_incomingChallengeThreads[i] = std::thread{ std::ref(*m_incomingChallengeHandlers[i]) };
	}
	for (size_t i = 0; i < k_numOutgoingThreads; i++) {
		m_outgoingChallengeThreads[i] = std::thread{ std::ref(*m_outgoingChallengeHandlers[i]) };
	}
	listen();
	for (int i = 0; i < k_numIncomingThreads; i++) {
		m_incomingChallengeHandlers[i]->kill();
		m_incomingChallengeThreads[i].join();
	}
	for (int i = 0; i < k_numOutgoingThreads; i++) {
		m_outgoingChallengeHandlers[i]->kill();
		m_outgoingChallengeThreads[i].join();
	}
}

void StreamHandler::kill() {
	m_running.store(false);
}

void StreamHandler::listen() {
	LOG("Awaiting challenges");
	const std::string_view header = Authorization::instance().getAuthorizationHeader();
	Curl curl = Curl::get(k_streamEventsURL, { header });
	curl.setCallback([this](std::string_view message) {
		return handleLine(message);
	});
	const CurlResponse response = curl.perform();
	if (!response.ok) {
		LOG("Failed to reach Lichess server");
	}
	LOG("Stream handler terminated");
}

bool StreamHandler::handleLine(std::string_view message) {
	const std::string_view header = Authorization::instance().getAuthorizationHeader();

	m_sharedState.notifyMainStreamStarted();

	if (!m_running.load()) {
		return CURL_BREAK;
	}
	if (message == "\n") {
		return CURL_CONTINUE;
	}
	json data;
	try {
		data = json::parse(message);
	}
	catch (...) {
		return CURL_CONTINUE;
	}

	if (data["type"] == "gameStart") {
		LOG("Game ", data["game"]["gameId"], " started");
		const std::string& fen = data["game"]["fen"];
		const Chess::PieceColor color = data["game"]["color"] == "white" ? Chess::PieceColor::White : Chess::PieceColor::Black;
		const std::string& id = data["game"]["gameId"];
		const int timePerSide = data["game"]["secondsLeft"].is_number_integer()
			? data["game"]["secondsLeft"].get<int>() * 1000
			: 1000000;
		m_sharedState.notifyGameStart({ .startFen = fen, .color = color, .id = id, .timePerSide = timePerSide });
	}
	else if (data["type"] == "gameFinish") {
	}
	else if (data["type"] == "challenge") {
		LOG("Received Challenge ", data["challenge"]["id"], " from ", data["challenge"]["challenger"]["name"]);
		if (data["challenge"]["variant"]["key"] != "standard") {
			LOG(data["challenge"]["id"], " has unsupported variant, rejecting");
			const std::string url = std::format(k_declineChallengeURL, data["challenge"]["id"].get<std::string>());
			const Curl post = Curl::post(url, { header }, { {"reason", "variant"} });
			post.perform();
		}
		else if (!contains(k_supportedTimeControls, data["challenge"]["speed"])) {
			LOG(data["challenge"]["id"], " has unsupported time control, rejecting");
			const std::string url = std::format(k_declineChallengeURL, data["challenge"]["id"].get<std::string>());
			const Curl post = Curl::post(url, { header }, { { "reason", "timeControl"} });
			post.perform();
		}
		else {
			LOG("Adding ", data["challenge"]["id"], " to queue");
			m_sharedState.enqueueChallenge(data["challenge"]["id"]);
		}
	}
	else if (data["type"] == "challengeCanceled") {
		LOG("Challenge ", data["challenge"]["id"], " was cancelled, removing from queue");
		m_sharedState.removeChallenge(data["challenges"]["id"]);
	}
	return CURL_CONTINUE;
}