/*------------------------------
| File: SceneSandbox.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "CaledonEngine/Systems/Engine/ISubsystem.h"
#include "CaledonEngine/Systems/Engine/IUpdatable.h"
#include "CaledonEngine/Systems/Rendering/IRenderClient.h"

#include <memory>
#include <string_view>

namespace CaledonEngine
{
	class EngineContext;
	class GameObject;
	class RenderingSubsystem;
}

namespace Game
{
	/*-----------------------------------------------------------------------------------------------
	| SceneSandbox: A temporary stand-in for the Scene. It creates the test GameObjects, updates them
	| every frame, and draws them inside the frame the RenderingSubsystem owns (as an IRenderClient).
	|
	| - Register it AFTER the RenderingSubsystem: it needs the renderer to exist, and shutdown runs in
	|   reverse order, so its GameObjects (and any textures they hold) are destroyed first.
	| - When the real Scene arrives, this class's job moves there and this file goes away.
	-----------------------------------------------------------------------------------------------*/
	class Sandbox final : public CaledonEngine::ISubsystem, public CaledonEngine::IUpdatable, public CaledonEngine::IRenderClient
	{
	private:
		CaledonEngine::RenderingSubsystem* m_pRendering;						// Non-owning: valid between Initialize() and Shutdown()
		std::unique_ptr<CaledonEngine::GameObject> m_pGameObject;				// The test GameObject

	public:
		static constexpr std::string_view kName = "SceneSandbox";				// Lookup key for EngineContext::Get<SceneSandbox>()

		Sandbox();															// Constructor
		~Sandbox() override;												// Destructor

		std::string_view Name() const noexcept override;						//
		bool Initialize(CaledonEngine::EngineContext& context) override;		// Builds the test scene
		void Shutdown() noexcept override;										// Destroys the test scene
		void Update(float deltaTime) override;									// Updates the GameObjects
		void OnRender(CaledonEngine::Renderer& renderer) override;				// Draws the GameObjects
	};
}