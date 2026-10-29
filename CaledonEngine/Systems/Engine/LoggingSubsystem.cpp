/*------------------------------
| File: LoggingSubsystem.cpp
| Author: Chandler Mays
------------------------------*/
#include "LoggingSubsystem.h"

#include <iostream>

/*---------------------------------------------------------------------------------------
| --- LevelToString:
---------------------------------------------------------------------------------------*/
namespace
{
	[[maybe_unused]] const char* LevelToString(CaledonEngine::LogLevel level) noexcept
	{
		switch (level)
		{
		case CaledonEngine::LogLevel::Error:	return "ERROR";
		case CaledonEngine::LogLevel::Warning:	return "WARNING";
		case CaledonEngine::LogLevel::Display:  return "DISPLAY";
		case CaledonEngine::LogLevel::Log:		return "LOG";
		}

		return "?";
	}
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*--------------------------------------------------------------------------
| --- Constructor: Constructs the LoggingSubsystem with default values --- |
--------------------------------------------------------------------------*/
CaledonEngine::LoggingSubsystem::LoggingSubsystem()
{}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::LoggingSubsystem::~LoggingSubsystem()
{
	// Log file is closed cleanly during Shutdown(), but if destroyed abruptly:
	std::lock_guard<std::mutex> lock{ m_mutex };
	if (m_logFile.is_open())
		m_logFile.close();
}

/*---------------------------------------------------------------------------------------
| --- Name:
---------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::LoggingSubsystem::Name() const noexcept
{
	return kName;
}

/*---------------------------------------------------------------------------------------
| --- Initiailze:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::LoggingSubsystem::Initialize([[maybe_unused]] EngineContext& context)
{
#ifdef _DEBUG
	const std::string& path = context.Config().logFilePath;
	if (!path.empty())
	{
		m_logFile.open(path);
		if (!m_logFile.is_open())
		{
			std::cerr << "[ERROR] [Logging] Failed to open '" << path << "'\n";
			return false;
		}
	}
#endif
	return true;
}

/*---------------------------------------------------------------------------------------
| --- Shutdown:
---------------------------------------------------------------------------------------*/
void CaledonEngine::LoggingSubsystem::Shutdown() noexcept
{
	try
	{
		std::lock_guard<std::mutex> lock{ m_mutex };
		if (m_logFile.is_open())
			m_logFile.close();
	}
	catch (...) {}
}

/*---------------------------------------------------------------------------------------
| --- Log:
---------------------------------------------------------------------------------------*/
void CaledonEngine::LoggingSubsystem::Log([[maybe_unused]] const std::string& message, [[maybe_unused]] bool newLine) noexcept
{
#ifdef _DEBUG
	try
	{
		std::lock_guard<std::mutex> lock{ m_mutex };

		std::cout << message;
		if (newLine) std::cout << '\n';

		if (m_logFile.is_open())
		{
			m_logFile << message;
			if (newLine) m_logFile << '\n';
			m_logFile.flush();
		}
	}
	catch (...) {}
#endif
}

/*---------------------------------------------------------------------------------------
| --- Log:
---------------------------------------------------------------------------------------*/
void CaledonEngine::LoggingSubsystem::Log([[maybe_unused]] LogLevel level, [[maybe_unused]] std::string_view category, [[maybe_unused]] std::string_view message) noexcept
{
#ifdef _DEBUG
	try
	{
		std::string line;
		line.reserve(category.size() + message.size() + 16);
		line += '[';
		line += LevelToString(level);
		line += "] [";
		line += category;
		line += "] ";
		line += message;

		Log(line, true);
	}
	catch (...) {}
#endif
}