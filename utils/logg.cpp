#include "logg.h"

std::mutex Logger::mtx;
LogLevel Logger::level = INFO;
std::ofstream Logger::logFile;
const std::string Logger::filename = "log.txt";
bool Logger::IsOutputToFile = false;

void Logger::Log(LogLevel level, const char* msg) {
    // TODO: Implement logging
    if(level < Logger::level) {
        return;
    }
    std::lock_guard<std::mutex> lock(mtx);
    std::string levelStr;
    switch (level) {
        case DEBUG:
            levelStr = "[DEBUG]";
            break;
        case INFO:
            levelStr = "[INFO]";
            break;
        case WARNING:
            levelStr = "[WARNING]";
            break;
        case ERROR:
            levelStr = "[ERROR]";
            break;
    }
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    std::string logMsg = "[" + std::string(buffer) + "]  " + levelStr + ": " + msg;
    if(IsOutputToFile && !logFile.is_open()) {
        logFile.open("log.txt", std::ios::app | std::ios::out);
    }
    if(IsOutputToFile && logFile.is_open()) {
        logFile << logMsg << std::endl;
    } else {
        std::cout << logMsg << std::endl;
    }
}

void Logger::SetLevel(LogLevel lev) {
    level = lev;
}

void Logger::SetLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx);
    logFile.close();
    logFile.open(filename, std::ios::app | std::ios::out);
}

void Logger::SetOutputToFile(bool outputToFile) {
    IsOutputToFile = outputToFile;
}
