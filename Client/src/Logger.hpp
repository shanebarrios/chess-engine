#pragma once

#include <mutex>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <format>
#include <string_view>

#define LOG(...) Logger::instance().log(__VA_ARGS__)

extern std::thread::id g_mainThreadId;

class Logger {
public:
	static Logger& instance() {
		static Logger s_logger{ std::cout };
		return s_logger;
	}

	template <typename... Args>
	void log(std::string_view format, Args&&... args);

	void setStream(std::ostream& out) { m_out = &out; }

private:
	std::mutex m_mutex{};
	std::ostream* m_out;

	Logger(std::ostream& out) : m_out{ &out } {}
};

template <typename... Args>
inline void Logger::log(std::string_view format, Args&&... args) {
	static constexpr std::string_view logFormat = "[{:%T}] [thread {}] {}\n";

	const auto time = std::chrono::system_clock::now();
	std::ostringstream ss {};
	ss << std::this_thread::get_id();
	{
		std::lock_guard lock {m_mutex};
		if constexpr (sizeof...(args) > 0)
		{
			(*m_out) << std::format(logFormat, time, ss.str(), std::vformat(format, std::make_format_args(std::forward<Args>(args)...)));
		}
		else
		{
			(*m_out) << std::format(logFormat, time, ss.str(), format);
		}
	}
}