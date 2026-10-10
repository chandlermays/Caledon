#pragma once
#include "../Engine/ISubsystem.h"
#include "../Engine/EngineContext.h"

#include <format>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>

namespace CaledonEngine
{
	enum class LogLevel
	{
		Error,
		Warning,
		Display,
		Log
	};

	class LoggingSubsystem final : public ISubsystem
	{
	private:
		std::mutex m_mutex;
		std::ofstream m_logFile;

	public:
		static constexpr std::string_view kName = "Logging";

		LoggingSubsystem();
		~LoggingSubsystem() override;

		std::string_view Name() const noexcept override;
		bool Initialize(EngineContext& context) override;
		void Shutdown() noexcept override;

		void Log(const std::string& message, bool newLine = true) noexcept;
		void Log(LogLevel level, std::string_view category, std::string_view message) noexcept;

		template<typename... Args>
			requires (sizeof...(Args) > 0)
		void Log([[maybe_unused]] std::format_string<Args...> fmt, [[maybe_unused]] Args&&... args) noexcept
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
		void Log(LogLevel level, std::string_view category, std::format_string<Args...> fmt, Args&&... args) noexcept
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