/*------------------------------
| File: WindowSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Engine/ISubsystem.h"

#include <memory>
#include <string_view>

namespace CaledonEngine
{
	class EngineContext;
	class Window;

	class WindowSubsystem final : public ISubsystem
	{
	private:
		std::unique_ptr<Window> m_pWindowAPI;											// The Platform Backend Window

	public:
		static constexpr std::string_view kName = "Window";								//

		WindowSubsystem();																// Constructor
		~WindowSubsystem() override;													// Destructor

		std::string_view Name() const noexcept override;								//
		bool Initialize(EngineContext& context) override;								//
		void Shutdown() noexcept override;												//

		Window* GetWindow() const noexcept;												//
	};
}