/*------------------------------
| File: RenderingSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IRenderable.h"

#include <memory>
#include <string_view>

namespace CaledonEngine
{
	class EngineContext;
	class Renderer;

	/*-----------------------------------------------------------------------------------------------
	| RenderingSubsystem: Owns the Renderer (replaces the legacy GraphicsManager). The window now
	| lives in its own WindowSubsystem; this one only draws into it.
	|
	| - Register it AFTER the WindowSubsystem. Initialize() fails (and logs why) if no window is up.
	|   Shutdown runs in reverse order, so the renderer is always destroyed before the window.
	| - It is an IRenderable, so EngineManager calls Render() once per frame.
	-----------------------------------------------------------------------------------------------*/
	class RenderingSubsystem final : public ISubsystem, public IRenderable
	{
	private:
		std::unique_ptr<Renderer> m_pRendererAPI;								// The Platform Backend Renderer

	public:
		static constexpr std::string_view kName = "Rendering";					//

		RenderingSubsystem();													// Constructor
		~RenderingSubsystem() override;											// Destructor

		std::string_view Name() const noexcept override;						//
		bool Initialize(EngineContext& context) override;						//
		void Shutdown() noexcept override;										//
		void Render() override;													//

		void BeginFrame();														//
		void EndFrame();														//

		Renderer* GetRenderer() const noexcept;									//
	};
}
