#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <sstream>
#include <thread>
#include <mutex>
#include <iostream>


enum LogLevel {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR
};
class Logger {
    private:
        static std::mutex mtx;
        static std::ofstream logFile;
        static LogLevel level;
        static bool IsOutputToFile;
        static const std::string filename;
    public:
        Logger() {}
        ~Logger() {}
        static void Log(LogLevel level, const char* msg);
        static void SetLevel(LogLevel lev);
        static void SetLogFile(const std::string& filename);
        static void SetOutputToFile(bool outputToFile);

};

#define ADEBUG(msg) Logger::Log(DEBUG, msg)
#define AINFO(msg) Logger::Log(INFO, msg)
#define AWARN(msg) Logger::Log(WARNING, msg)
#define AERROR(msg) Logger::Log(ERROR, msg)
#define OUTPUT_TO_FILE Logger::SetOutputToFile(true)
#define OUTPUT_TO_CONSOLE Logger::SetOutputToFile(false)