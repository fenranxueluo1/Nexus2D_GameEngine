#pragma once
#include "Logger.h"
#include <chrono>
#include <ctime>
#include <format>
#include <iostream>
#include <sstream>

#if defined(_WIN32)
#   ifndef NOMINMAX
#       define NOMINMAX            // 避免 Windows.h 的 min/max 宏污染 std 命名空间
#   endif
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <Windows.h>
#endif

namespace NEXUS_LOGGER {

	// ---------- 跨平台控制台着色 ----------
	// Windows : Win32 控制台文本属性（SetConsoleTextAttribute）
	// 其它平台: ANSI 转义序列（绝大多数 Linux / macOS 终端均支持）
	//
	// 原先的 GREEN / RED / YELLOW / WHITE 是 Win32 的 WORD 属性位值，
	// 在 Linux 上没有意义，这里改为与平台无关的颜色枚举。
	enum class ConsoleColor { Green, Red, Yellow, White };

	inline void SetConsoleColor(ConsoleColor color)
	{
#if defined(_WIN32)
		WORD attribute = 7; // WHITE
		switch (color)
		{
		case ConsoleColor::Green:  attribute = 2; break;
		case ConsoleColor::Red:    attribute = 4; break;
		case ConsoleColor::Yellow: attribute = 6; break;
		case ConsoleColor::White:  attribute = 7; break;
		}
		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), attribute);
#else
		const char* code = "0";
		switch (color)
		{
		case ConsoleColor::Green:  code = "32"; break;
		case ConsoleColor::Red:    code = "31"; break;
		case ConsoleColor::Yellow: code = "33"; break;
		case ConsoleColor::White:  code = "0";  break;
		}
		std::cout << "\033[" << code << "m";
#endif
	}

	inline void ResetConsoleColor()
	{
#if defined(_WIN32)
		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), 7); // WHITE
#else
		std::cout << "\033[0m";
#endif
	}

	template <typename... Args>
	void Logger::Log(const std::string& message, Args&&... args)
	{
		assert(m_bInitialized && "The logger must be initialized before it is used!");

		if (!m_bInitialized)
		{
			std::cout << "The logger must be initialized before it is used!" << std::endl;
			return;
		}

		std::stringstream ss;
		ss << "NEXUS [INFO]: " << CurrentDateTime() << " - " << std::vformat(message, std::make_format_args(args...)) << "\n";

		if (m_bConsoleLog)
		{
			SetConsoleColor(ConsoleColor::Green);
			std::cout << ss.str();
			ResetConsoleColor();
		}

		if (m_bRetainLogs)
			m_LogEntries.emplace_back(LogEntry::LogType::INFO, ss.str());
	}

	template <typename... Args>
	void Logger::Warn(const std::string& message, Args&&... args)
	{

		assert(m_bInitialized && "The logger must be initialized before it is used!");

		if (!m_bInitialized)
		{
			std::cout << "The logger must be initialized before it is used!" << std::endl;
			return;
		}

		std::stringstream ss;
		ss << "NEXUS [WARN]: " << CurrentDateTime() << " - " << std::vformat(message, std::make_format_args(args...)) << "\n";

		if (m_bConsoleLog)
		{
			SetConsoleColor(ConsoleColor::Yellow);
			std::cout << ss.str();
			ResetConsoleColor();
		}

		if (m_bRetainLogs)
			m_LogEntries.emplace_back(LogEntry::LogType::WARN, ss.str());
	}

	template <typename... Args>
	void Logger::Error(std::source_location location, const std::string& message, Args&&... args)
	{
		assert(m_bInitialized && "The logger must be initialized before it is used!");

		if (!m_bInitialized)
		{
			std::cout << "The logger must be initialized before it is used!" << std::endl;
			return;
		}

		std::stringstream ss;
		ss << "NEXUS [ERROR]: " << CurrentDateTime() << " - " << std::vformat(message, std::make_format_args(args...)) <<
			"\nFUNC: " << location.function_name() <<
			"\nLINE: " << location.line() << "\n\n";

		if (m_bConsoleLog)
		{
			SetConsoleColor(ConsoleColor::Red);
			std::cout << ss.str();
			ResetConsoleColor();
		}

		if (m_bRetainLogs)
			m_LogEntries.emplace_back(LogEntry::LogType::ERR, ss.str());
	}
}
