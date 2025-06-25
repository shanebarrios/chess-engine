#include "Curl.hpp"

#include <sstream>

void Curl::init() {
	curl_global_init(CURL_GLOBAL_ALL);
}

void Curl::cleanup() {
	curl_global_cleanup();
}

size_t Curl::writeCallback(void* buffer, size_t size, size_t nmemb, void* userp) {
	Curl* curl = static_cast<Curl*>(userp);
	if (curl->m_userCallback) {
		std::string s = std::string{ static_cast<char*>(buffer), size * nmemb };
		
		if (!curl->m_userCallback(s)) {
			return 0;
		}
	}
	return size * nmemb;
}

Curl Curl::get(std::string_view url, const std::vector<std::string_view>& headers) {
	Curl curl{};
	curl.m_handle = curl_easy_init();
	curl_easy_setopt(curl.m_handle, CURLOPT_URL, url.data());
	curl_easy_setopt(curl.m_handle, CURLOPT_WRITEFUNCTION, Curl::writeCallback);
	curl_easy_setopt(curl.m_handle, CURLOPT_WRITEDATA, static_cast<void*>(&curl));

	for (std::string_view s : headers) {
		curl.m_headers = curl_slist_append(curl.m_headers, s.data());
	}

	if (curl.m_headers != nullptr) {
		curl_easy_setopt(curl.m_handle, CURLOPT_HTTPHEADER, curl.m_headers);
	}

	return curl;
}

Curl Curl::post(std::string_view url, const std::vector<std::string_view>& headers, const std::map<std::string_view, std::string_view>& body) {
	Curl curl = Curl::get(url, headers);

	std::stringstream ss{};

	bool first = true;
	for (const auto& [key, val] : body) {
		if (!first) {
			ss << '&';
		}
		ss << key << '=' << val;
		first = false;
	}

	const std::string s = ss.str();
	curl_easy_setopt(curl.m_handle, CURLOPT_COPYPOSTFIELDS, s.c_str());

	return curl;
}

Curl::~Curl() {
	if (m_headers) {
		curl_slist_free_all(m_headers);
	}
	if (m_handle) {
		curl_easy_cleanup(m_handle);
	}
}

Curl::Curl(Curl&& other) noexcept : m_userCallback{ std::move(other.m_userCallback) }, m_handle{ other.m_handle }, m_headers{ other.m_headers } {
	other.m_userCallback = {};
	other.m_handle = nullptr;
	other.m_headers = nullptr;
}

Curl& Curl::operator=(Curl&& other) noexcept {
	if (&other == this) {
		return *this;
	}

	if (m_headers) {
		curl_slist_free_all(m_headers);
	}

	if (m_handle) {
		curl_easy_cleanup(m_handle);
	}

	m_userCallback = std::move(other.m_userCallback);
	m_handle = other.m_handle;
	m_headers = other.m_headers;

	other.m_userCallback = {};
	other.m_handle = nullptr;
	other.m_headers = nullptr;

	return *this;
}

void Curl::setCallback(CurlCallback callback) {
	m_userCallback = std::move(callback);
}

CurlResponse Curl::perform() const {
	const CURLcode code = curl_easy_perform(m_handle);
	long responseCode = 0;
	curl_easy_getinfo(m_handle, CURLINFO_RESPONSE_CODE, &responseCode);
	return CurlResponse{ code == CURLE_OK ? true : false, static_cast<int>(responseCode) };
}

