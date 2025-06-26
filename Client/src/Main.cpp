#include <iostream>
#include <string>
#include "StreamHandler.hpp"
#include "Curl.hpp"
#include "Logger.hpp"
#include <Chess.hpp>

std::thread::id g_mainThreadId;

int main(int argc, char** argv) {
	LOG("Initializing chess engine");
	Chess::init();
	Curl::init();

	g_mainThreadId = std::this_thread::get_id();
	
	StreamHandler handler{};

	handler.run();

	Curl::cleanup();

	return 0;
}
