//#pragma once

#include <string_view>
#include <vector>
#include <functional>
#include <map>
#include <curl/curl.h>

#define CURL_CONTINUE true
#define CURL_BREAK false

using CurlCallback = std::function<bool(std::string_view)>;

struct CurlResponse {
	bool ok;
	int responseCode;
};

class Curl {
public:
	static void init();
	static void cleanup();

	static Curl get(std::string_view url, const std::vector<std::string_view>& headers = {});
	static Curl post(std::string_view url, const std::vector<std::string_view>& headers = {}, const std::map<std::string_view, std::string_view>& body = {});

	~Curl(); 
	Curl(const Curl&) = delete;
	Curl& operator=(const Curl&) = delete;
	Curl(Curl&&) noexcept;
	Curl& operator=(Curl&&) noexcept;

	void setCallback(CurlCallback callback);

	// Blocking operation
	CurlResponse perform() const;
private:
	static size_t writeCallback(void* buffer, size_t size, size_t nmemb, void* userp);

	Curl() = default;

	CurlCallback m_userCallback{};

	CURL* m_handle = nullptr;
	struct curl_slist* m_headers = nullptr;
};

