#pragma once

#include <string_view>
#include <string>
#include <iostream>
#include <cstdlib>

class Authorization
{
public:
	static const Authorization& instance() {
		static Authorization auth;
		return auth;
	}

	std::string_view getAuthorizationHeader() const {
		return m_authorizationHeader;
	}

private:
	std::string m_authorizationHeader;

	Authorization() {
		const char* apiToken = std::getenv("LICHESS_API_TOKEN");
		if (apiToken == nullptr) {
			std::cerr << "Missing Lichess API Token! Set LICHESS_API_TOKEN environment variable\n";
			std::exit(1);
		}
		m_authorizationHeader = "Authorization: Bearer " + std::string{ apiToken };
	}
};