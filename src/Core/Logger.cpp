#include "Core/Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

namespace FinalStorm {

Logger::LogLevel Logger::currentLevel = Logger::LogLevel::Info;
std::mutex Logger::logMutex;
std::ofstream Logger::fileStream;

void Logger::initialize(LogLevel level, const std::string& filename) {
    currentLevel = level;
    
    if (!filename.empty()) {
        fileStream.open(filename, std::ios::out | std::ios::app);
        if (!fileStream.is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
        }
    }
}

void Logger::shutdown() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(logMutex);
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";
    ss << "[" << logLevelToString(level) << "] ";
    ss << message;
    
    std::string logLine = ss.str();
    
    // Output to console with color
    switch (level) {
        case LogLevel::Error:
            std::cerr << "\033[31m" << logLine << "\033[0m" << std::endl;
            break;
        case LogLevel::Warning:
            std::cout << "\033[33m" << logLine << "\033[0m" << std::endl;
            break;
        case LogLevel::Info:
            std::cout << "\033[37m" << logLine << "\033[0m" << std::endl;
            break;
        case LogLevel::Debug:
            std::cout << "\033[36m" << logLine << "\033[0m" << std::endl;
            break;
    }
    
    // Output to file
    if (fileStream.is_open()) {
        fileStream << logLine << std::endl;
        fileStream.flush();
    }
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        default: return "UNKNOWN";
    }
}

} // namespace FinalStorm