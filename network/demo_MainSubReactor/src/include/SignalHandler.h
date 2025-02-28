#pragma once
#include <signal.h>
#include <functional>
#include <map>

std::map<int, std::function<void()>> handles_;
void signal_handler(int sig) {
    handles_[sig]();
}
struct Signal {
    static void signal(int sig, const std::function<void()> &handle) {
        handles_[sig] = handle;
        ::signal(sig, signal_handler);
    }
};