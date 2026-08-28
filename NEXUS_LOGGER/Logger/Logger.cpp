#include "Logger.h"

#include <chrono>
#include <ctime>
#include <format>
#include <iostream>

namespace NEXUS_LOGGER {

    Logger::LogTime::LogTime(const std::string& date)
        : day{date.substr(0, 3)}, dayNumber{date.substr(8, 2)}, month{date.substr(4, 3)}
        , year{ date.substr(20, 4)}, time{date.substr(11, 8)}
    {

    }

    std::string Logger::CurrentDateTime()
    {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        // ctime_s 是 MSVC 专有扩展，Linux 下不可用。
        // 改用标准的 localtime + strftime，并按原先 ctime_s 的输出格式
        // "Www Mmm dd hh:mm:ss yyyy" 生成字符串，以保证 LogTime 的解析偏移不变。
        std::tm localTime{};
#if defined(_WIN32)
        localtime_s(&localTime, &time);   // MSVC 参数顺序：(tm*, const time_t*)
#else
        localtime_r(&time, &localTime);   // POSIX 参数顺序：(const time_t*, tm*)
#endif

        char buf[30];
        std::strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y", &localTime);

        LogTime logTime{ std::string{buf} };
        return std::format("{0}-{1}-{2} {3}", logTime.year, logTime.month, logTime.dayNumber, logTime.time);
    }

    Logger& Logger::GetInstance()
    {
        static Logger instance{};
        return instance;
    }

    void Logger::Init(bool consoleLog, bool retainLogs)
    {
        assert(!m_bInitialized && "Don not call Initialize more than once!");
        
        if (m_bInitialized)
        {
            std::cout << "Logger has already been initialized!" << std::endl;
            return;
        }

        m_bConsoleLog = consoleLog;
        m_bRetainLogs = retainLogs;
        m_bInitialized = true;
    }
}
