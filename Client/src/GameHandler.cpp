#include "GameHandler.hpp"
#include "Curl.hpp"
#include "Logger.hpp"
#include "Authorization.hpp"

static constexpr std::string_view k_streamGameURL = "https://lichess.org/api/bot/game/stream/{}";
static constexpr std::string_view k_makeMoveURL = "https://lichess.org/api/bot/game/{}/move/{}";

static constexpr float k_thinkMultiplier = 0.005;

GameHandler::GameHandler(const GameStartEvent& gameStart) :
	m_position{ Chess::Position::fromFen(gameStart.startFen) },
	m_color{ gameStart.color },
	m_id{ gameStart.id },
	m_timePerSide{ gameStart.timePerSide }  {
}

void GameHandler::operator()() {
	run();
}

void GameHandler::run() {
	LOG("Handling game {}", m_id);
	int numMoves = 0;

	const std::string url = std::format(k_streamGameURL, m_id);
	std::string_view header = Authorization::instance().getAuthorizationHeader();
	Curl curl = Curl::get(url, { header });
	curl.setCallback([this, &numMoves](std::string_view s) {
		if (s == "\n") {
			return CURL_CONTINUE;
		}

		json data;

		try {
			data = json::parse(s);
		}
		catch (...) {
			return CURL_CONTINUE;
		}

		if (data["type"] != "gameFull" && data["type"] != "gameState") {
			return CURL_CONTINUE;
		}

		const json& state = data["type"] == "gameFull" ? data["state"] : data;

		if (state["status"] == "finished") {
			return CURL_BREAK;
		}

		const std::string& moveStr = state["moves"].get<std::string>();
		const std::vector<std::string> split = Chess::Utils::split(moveStr);

		if (split.size() > numMoves) {
			for (int i = numMoves; i < split.size(); i++) {
				LOG("Received move {}", split[i]);
				m_position.makeMove(Chess::Utils::strToMove(split[i]));
			}
			numMoves = static_cast<int>(split.size());
		}

		if (m_position.getTurn() == m_color) {
			sendMove();
		}

		return CURL_CONTINUE;
		});
	curl.perform();
	LOG("Game {} finished", m_id);
}

bool GameHandler::sendMove() {
	const int thinkTimeMS = std::min(5000, static_cast<int>(k_thinkMultiplier * m_timePerSide));
	const Chess::Move move = m_searcher.getMove(m_position, thinkTimeMS);
	const std::string url = std::format(k_makeMoveURL, m_id, Chess::Utils::moveToStr(move));
	const std::string_view header = Authorization::instance().getAuthorizationHeader();
	const Curl curl = Curl::post(url, { header }, {});
	const CurlResponse response = curl.perform();
	if (!response.ok) {
		LOG("Failed to reach Lichess server");
		return false;
	}
	else if (response.responseCode != 200) {
		LOG("Move {} was invalid, failed to send to server", Chess::Utils::moveToStr(move));
		return false;
	}
	else {
		LOG("Sent move {} to server", Chess::Utils::moveToStr(move));
		return true;
	}
}