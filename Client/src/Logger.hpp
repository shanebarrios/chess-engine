#pragma once

#include <mutex>
#include <iostream>
#include <sstream>
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
	auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	ss << std::format("[{:%T}] ", time);

	const std::thread::id curThreadId = std::this_thread::get_id();
	if (curThreadId == g_mainThreadId) {
		ss << "[main] ";
	}
	else {
		ss << "[Thread " << curThreadId << "] ";
	}
	(ss << ... << std::forward<Args>(args));
	ss << '\n';
	{
		std::lock_guard lock{ m_mutex };
		(*m_out) << ss.str();
	}
}