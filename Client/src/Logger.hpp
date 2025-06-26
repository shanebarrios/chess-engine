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
	void log(Args&&... args);

	void setStream(std::ostream& out) { m_out = &out; }

private:
	std::mutex m_mutex{};
	std::ostream* m_out;

	Logger(std::ostream& out) : m_out{ &out } {}
};

template <typename... Args>
inline void Logger::log(Args&&... args) {
	std::ostringstream ss{};
	auto now = std::chrono::system_clock::now();
	auto time_t_now = std::chrono::system_clock::to_time_t(now);
	std::tm local_tm = *std::localtime(&time_t_now);
	ss << std::put_time(&local_tm, "%H:%M:%S");

	const std::thread::id curThreadId = std::this_thread::get_id();
	if (curThreadId == g_mainThreadId) {
		ss << " [main] ";
	}
	else {
		ss << " [Thread " << curThreadId << "] ";
	}
	(ss << ... << std::forward<Args>(args));
	ss << '\n';
	{
		std::lock_guard lock{ m_mutex };
		(*m_out) << ss.str();
	}
}