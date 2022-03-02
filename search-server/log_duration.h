#pragma once

#include <chrono>
#include <iostream>

#define PROFILE_CONCAT_INTERNAL(X, Y) X##Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(name) LogDuration UNIQUE_VAR_NAME_PROFILE(x)
#define LOG_DURATION_STREAM(name, stream) LogDuration UNIQUE_VAR_NAME_PROFILE(name, stream)

class LogDuration {
public:
	using Clock = std::chrono::steady_clock;

	LogDuration(const std::string& id, std::ostream& stream = std::cerr) : id_(id), output_(stream) {}

	~LogDuration() {
		using namespace std::chrono;

		const auto end_time = Clock::now();
		const auto duration = end_time - start_time_;

		output_ << id_ << ": " << duration_cast<milliseconds>(duration).count() << " ms" << std::endl;
	}

private:
	const std::string id_;
	const Clock::time_point start_time_ = Clock::now();
	std::ostream& output_;
};

