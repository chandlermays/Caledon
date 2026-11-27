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

	/*-------------------------------------------------------------------------------------
	| --- WindowSubsystem: Owns the application window and opens it through the       --- |
	| ---                  platform backend. The window is kept only once it has      --- |
	| ---                  fully opened, so a failed Initialize leaves nothing behind --- |
	-------------------------------------------------------------------------------------*/
	class WindowSubsystem final : public ISubsystem
	{
	private:
		WindowConfig m_config;															// How the window is opened
		std::unique_ptr<Window> m_pWindowAPI;											// The Platform Backend Window

	public:
		static constexpr std::string_view kName = "Window";								// Unique name used for registration and lookup

		explicit WindowSubsystem(const WindowConfig& config = WindowConfig{});			// Constructor
		~WindowSubsystem() override;													// Destructor

		std::string_view Name() const noexcept override;								// Returns the Subsystem's name
		bool Initialize(EngineContext& context) override;								// Opens the window through the platform backend
		void Shutdown() noexcept override;												// Closes and destroys the window

		Window* GetWindow() const noexcept;												// Returns the window, or nullptr if it isn't open
	};
}
