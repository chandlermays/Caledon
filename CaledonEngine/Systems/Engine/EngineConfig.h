#pragma once
#include <cstdint>
#include <string>

namespace CaledonEngine
{
	enum class PlatformBackend
	{
		SDL3,
		//... set up for future expansions
	};

	struct EngineConfig
	{
		PlatformBackend backendAPI = PlatformBackend::SDL3;				// The API Configuration for Rendering, Input, etc.

		std::uint32_t targetFps = 60;									// Frame pacing target (0 = uncapped)
		float maxFrameTimeSeconds = 0.25f;								// Clamp for deltaTime
		std::uint64_t maxFrames = 0;									// Quit after N frames (0 = run until quit is requested)

		std::uint32_t fixedUpdateHz = 60;								// IFixedUpdatable rate in steps per second (0 = fixed update disabled)
		std::uint32_t maxFixedStepsPerFrame = 5;						// Cap on catch-up steps in one frame; any backlog beyond it is discarded

		std::string logFilePath = "CaledonEngine.txt";					// Debug log file
	};
}