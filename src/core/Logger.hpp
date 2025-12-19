#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace Core {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

/// Simple logging utility - can be extended to file logging, etc.
class Logger {
public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }
    
    void setMinLevel(LogLevel level) { m_minLevel = level; }
    
    template<typename... Args>
    void log(LogLevel level, const std::string& category, Args&&... args) {
        if (level < m_minLevel) return;
        
        std::ostringstream oss;
        oss << getTimestamp() << " [" << levelToString(level) << "] [" << category << "] ";
        (oss << ... << std::forward<Args>(args));
        oss << "\n";
        
        if (level >= LogLevel::Error) {
            std::cerr << oss.str();
        } else {
            std::cout << oss.str();
        }
    }
    
    template<typename... Args>
    void debug(const std::string& category, Args&&... args) {
        log(LogLevel::Debug, category, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void info(const std::string& category, Args&&... args) {
        log(LogLevel::Info, category, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warning(const std::string& category, Args&&... args) {
        log(LogLevel::Warning, category, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void error(const std::string& category, Args&&... args) {
        log(LogLevel::Error, category, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void fatal(const std::string& category, Args&&... args) {
        log(LogLevel::Fatal, category, std::forward<Args>(args)...);
    }

private:
    Logger() = default;
    
    LogLevel m_minLevel = LogLevel::Info;
    
    static const char* levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
        }
        return "UNKNOWN";
    }
    
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }
};

// Convenience macros
#define LOG_DEBUG(category, ...) Core::Logger::instance().debug(category, __VA_ARGS__)
#define LOG_INFO(category, ...) Core::Logger::instance().info(category, __VA_ARGS__)
#define LOG_WARN(category, ...) Core::Logger::instance().warning(category, __VA_ARGS__)
#define LOG_ERROR(category, ...) Core::Logger::instance().error(category, __VA_ARGS__)
#define LOG_FATAL(category, ...) Core::Logger::instance().fatal(category, __VA_ARGS__)

} // namespace Core
