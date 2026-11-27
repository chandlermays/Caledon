/*------------------------------
| File: LoggingSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Engine/EngineContext.h"
#include "Systems/Engine/ISubsystem.h"

#include <format>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>

namespace CaledonEngine
{
	/*----------------------------------------------------------------------
	| --- LogLevel: How serious a logged message is, most severe first --- |
	----------------------------------------------------------------------*/
	enum class LogLevel
	{
		Error,																						// Something failed
		Warning,																					// Something looks wrong but execution continues
		Display,																					// Information worth showing the user
		Log																							// General diagnostic output
	};

	/*------------------------------------------------------------------------------------------
	| --- LoggingSubsystem: Writes engine messages to the console and a log file.          --- |
	| ---                   Logging only happens in debug builds; in release builds the    --- |
	| ---                   CE_LOG macro compiles to nothing, so its arguments are never   --- |
	| ---                   evaluated. Writes are guarded by a mutex so any thread can log --- |
	------------------------------------------------------------------------------------------*/
	class LoggingSubsystem final : public ISubsystem
	{
	private:
		std::mutex m_mutex;																			// Guards the log file and console output across threads
		std::ofstream m_logFile;																	// The file every message is written to

	public:
		static constexpr std::string_view kName = "Logging";										// Unique name used for registration and lookup

		LoggingSubsystem();																			// Constructor
		~LoggingSubsystem() override;																// Destructor

		std::string_view Name() const noexcept override;											// Returns the Subsystem's name
		bool Initialize(EngineContext& context) override;											// Opens the log file named in the config, if any
		void Shutdown() noexcept override;															// Closes the log file

		void Log(const std::string& message, bool newLine = true) noexcept;							// Writes a raw message
		void Log(LogLevel level, std::string_view category, std::string_view message) noexcept;		// Writes a message tagged with a level and category

		template<typename... Args>
			requires (sizeof...(Args) > 0)
		void Log([[maybe_unused]] std::format_string<Args...> fmt, [[maybe_unused]] Args&&... args) noexcept	// Formats and writes a raw message (debug builds only)
		{
#ifdef _DEBUG
			try
			{
				Log(std::format(fmt, std::forward<Args>(args)...));
			}
			catch (...)
			{
				//...
			}
#endif
		}

		template<typename... Args>
			requires (sizeof...(Args) > 0)
		void Log(LogLevel level, std::string_view category, std::format_string<Args...> fmt, Args&&... args) noexcept	// Formats and writes a tagged message (debug builds only)
		{
#ifdef _DEBUG
			try
			{
				Log(level, category, std::format(fmt, std::forward<Args>(args)...));
			}
			catch (...)
			{
				//...
			}
#endif
		}
	};
}

#ifdef _DEBUG
#define CE_LOG(context, ...) \
		do { \
			if (auto* pLogger = (context).Get<CaledonEngine::LoggingSubsystem>()) { \
				pLogger->Log(__VA_ARGS__); \
			} \
		} while(false)
#else
#define CE_LOG(context, ...) do {} while(false)
#endif