/*------------------------------
| File: SceneSandbox.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "CaledonEngine/Systems/Engine/ISubsystem.h"
#include "CaledonEngine/Systems/Engine/IUpdatable.h"

#include <memory>
#include <string_view>

namespace CaledonEngine
{
	class EngineContext;
	class GameObject;
	class Scene;
}

namespace Game
{
	class Sandbox final : public CaledonEngine::ISubsystem, public CaledonEngine::IUpdatable
	{
	private:
		CaledonEngine::EngineContext* m_pContext = nullptr;						// The context used to reach the other subsystems
		CaledonEngine::Scene* m_pActiveScene = nullptr;							// The Sandbox scene that is currently active
		int m_activeSandbox = 0;												// Which Sandbox (1, 2, 3, 4 or 5) is active. 0 means none
		CaledonEngine::GameObject* m_pRoller = nullptr;							// The rolling Player of the active Sandbox, if it has one. Owned by the Scene
		CaledonEngine::GameObject* m_pRollMarker = nullptr;						// The dot that rides on the rim of the roller so its rotation is visible. Owned by the Scene

		bool LoadSandbox(int sandbox);											// Destroys the active Sandbox scene and builds the requested one in its place

	public:
		static constexpr std::string_view kName = "SceneSandbox";				//

		std::string_view Name() const noexcept override;						//
		bool Initialize(CaledonEngine::EngineContext& context) override;		//
		void Shutdown() noexcept override;										//
		void Update(float deltaTime) override;									// Switches Sandbox when the 1, 2, 3, 4 or 5 key is pressed and moves the roll marker
	};
}