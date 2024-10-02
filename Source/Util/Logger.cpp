
#include "Logger.h"

void LineOfSightAnalyzer::Logger::Log(LogLevel level, const std::string& message)
{
    thread_local const auto TL_THREAD_ID = mLastThreadId.fetch_add(1);

    const auto log = std::format("{:.12} [{:}] [ {:5} ] {:}", GetTimeString(), TL_THREAD_ID, GetLogLevelString(level), message);

    switch (level)
    {
        case LogLevel::TRACE:
        case LogLevel::DEBUG:

        case LogLevel::INFO:
            std::cout << log << std::endl;
            break;
        case LogLevel::WARNING:
        case LogLevel::FATAL:
            std::cerr << log << std::endl;
            break;
        default:
            break;
    }
}

void LineOfSightAnalyzer::Logger::SetLogLevel(LogLevel logLevel)
{
    mLogLevel = logLevel;
}

LineOfSightAnalyzer::LogLevel LineOfSightAnalyzer::Logger::GetLogLevel()
{
    return mLogLevel;
}

std::string LineOfSightAnalyzer::Logger::GetTimeString()
{
    const auto zone = std::chrono::current_zone();
    const auto now = std::chrono::system_clock::now();
    const auto local = std::chrono::zoned_time(zone, now);

    return std::format("{:%T}", local);
}

bool LineOfSightAnalyzer::Logger::isLogEnabledFor(LogLevel level)
{
    return mLogLevel <= level;
}

LineOfSightAnalyzer::LogLevel LineOfSightAnalyzer::Logger::mLogLevel = LineOfSightAnalyzer::LogLevel::ALL;

std::atomic_uint32_t LineOfSightAnalyzer::Logger::mLastThreadId = 0;