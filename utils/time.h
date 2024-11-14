#pragma once 

#include <string>
#include <chrono>

class Timer {
public:
    Timer() = default;
    ~Timer() {}
    void start();
    void stop();
    void reset();
    long long elapsed() const;
    double elapsed_seconds() const;
    double elapsed_minutes() const;
    std::string elapsed_str() const;
    std::string elapsed_seconds_str() const;
    std::string elapsed_minutes_str() const;
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
    std::chrono::time_point<std::chrono::high_resolution_clock> stop_time_;
    std::chrono::milliseconds total_time_;
    std::chrono::seconds total_seconds_;
    bool is_running_ = false;
};