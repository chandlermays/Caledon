/*------------------------------
| File: WindowSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Engine/ISubsystem.h"
#include "Systems/Window/WindowConfig.h"

#include <memory>
#include <string_view>

namespace CaledonEngine
{
	class EngineContext;
	class Window;

	class WindowSubsystem final : public ISubsystem
	{
	private:
		WindowConfig m_config;															// How the window is opened
		std::unique_ptr<Window> m_pWindowAPI;											// The Platform Backend Window

	public:
		static constexpr std::string_view kName = "Window";								//

		explicit WindowSubsystem(const WindowConfig& config = WindowConfig{});			// Constructor
		~WindowSubsystem() override;													// Destructor

		std::string_view Name() const noexcept override;								//
		bool Initialize(EngineContext& context) override;								//
		void Shutdown() noexcept override;												//

		Window* GetWindow() const noexcept;												//
	};
}
