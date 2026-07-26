#pragma once

#include <assert.h>
#include <atomic>
#include <format>
#include <map>
#include <string>

#include <QDebug>

namespace LineOfSightAnalyzer
{
    enum class LogLevel
    {
        ALL = -1,
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        FATAL = 4,
        NONE = 5
    };

    class Logger
    {
      public:
        Logger() = delete;

        static void Log(LogLevel LogLevel, const std::string& LogMessage);
        static void SetLogLevel(LogLevel LogLevel);
        static bool IsLogEnabledFor(LogLevel LogLevel);
        static LogLevel GetLogLevel();
        static std::string GetTimeString();
        static void QtMessageOutputCallback(QtMsgType Type, const QMessageLogContext& Context, const QString& LogMessage);
        static std::string GetLogLevelString(LogLevel LogLevel);

      private:
        static LogLevel mLogLevel;
        static std::atomic_uint32_t mLastThreadId;
    };
}

#define LOG_PRIVATE(LEVEL, FORMAT, ...) \
    do \
    { \
        if (LineOfSightAnalyzer::Logger::IsLogEnabledFor(LEVEL)) \
        { \
            LineOfSightAnalyzer::Logger::Log(LEVEL, std::format(FORMAT, __VA_ARGS__)); \
        } \
    } while (false)

#define LOG_TRACE(FORMAT, ...) LOG_PRIVATE(LineOfSightAnalyzer::LogLevel::TRACE, FORMAT, __VA_ARGS__)
#define LOG_DEBUG(FORMAT, ...) LOG_PRIVATE(LineOfSightAnalyzer::LogLevel::DEBUG, FORMAT, __VA_ARGS__)
#define LOG_INFO(FORMAT, ...) LOG_PRIVATE(LineOfSightAnalyzer::LogLevel::INFO, FORMAT, __VA_ARGS__)
#define LOG_WARN(FORMAT, ...) LOG_PRIVATE(LineOfSightAnalyzer::LogLevel::WARNING, FORMAT, __VA_ARGS__)
#define LOG_FATAL(FORMAT, ...) LOG_PRIVATE(LineOfSightAnalyzer::LogLevel::FATAL, FORMAT, __VA_ARGS__)

#define LOS_ASSERT(EXPRESSION) assert(EXPRESSION)

#define LOS_EXIT_FAILURE(FORMAT, ...) \
    do \
    { \
        LOG_FATAL(FORMAT, __VA_ARGS__); \
        std::exit(EXIT_FAILURE); \
    } while (false)

#define LOS_EXIT_FAILURE_IF(CONDITION, FORMAT, ...) \
    do \
    { \
        if (CONDITION) \
        { \
            LOG_FATAL(FORMAT, __VA_ARGS__); \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)