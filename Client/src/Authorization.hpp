#pragma once

#include <string_view>
#include <string>
#include <iostream>

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
		char* apiToken = nullptr;
		size_t length = 0;
		errno_t err = _dupenv_s(&apiToken, &length, "LICHESS_API_TOKEN");
		if (err != 0 || apiToken == nullptr) {
			std::cerr << "Missing Lichess API Token! Set LICHESS_API_TOKEN environment variable\n";
			std::exit(1);
		}
		m_authorizationHeader = "Authorization: Bearer " + std::string{ apiToken };
		free(apiToken);
	}
};