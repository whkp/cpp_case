#include "time.h"

void Timer::start() {
    start_time_ = std::chrono::high_resolution_clock::now();
    is_running_ = true;
}

void Timer::stop() {
    if(!is_running_) {
        return;
    }
    stop_time_ = std::chrono::high_resolution_clock::now();
    is_running_ = false;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_ - start_time_);
    total_time_ += duration;
}

long long Timer::elapsed() const {
    if(is_running_) {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time_);
        return total_time_.count() + duration.count();
    } else {
        return total_time_.count();
    }
}

double Timer::elapsed_seconds() const {
    return elapsed() / 1000;
}

double Timer::elapsed_minutes() const {
    return elapsed_seconds() / 60;
}

std::string Timer::elapsed_str() const {
    return std::to_string(elapsed()) + "ms";
}

std::string Timer::elapsed_seconds_str() const {
    return std::to_string(elapsed_seconds()) + "s";
}

std::string Timer::elapsed_minutes_str() const {
    return std::to_string(elapsed_minutes()) + "min";
}