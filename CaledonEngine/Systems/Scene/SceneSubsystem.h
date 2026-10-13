/*------------------------------
| File: SceneSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IUpdatable.h"

#include <memory>
#include <string_view>
#include <vector>

namespace CaledonEngine
{
	class EngineContext;
	class Scene;

	class SceneSubsystem final : public ISubsystem, public IUpdatable
	{
	private:
		std::vector<std::unique_ptr<Scene>> m_pScenes;								//
		Scene* m_pCurrentScene;														//

	public:
		static constexpr std::string_view kName = "Scene";							//

		SceneSubsystem();															// Constructor
		~SceneSubsystem() override;													// Destructor
		SceneSubsystem(const SceneSubsystem&) = delete;								// Prevent copy-construction
		SceneSubsystem& operator=(const SceneSubsystem&) = delete;					// Prevent copy-assignment
		SceneSubsystem(SceneSubsystem&&) = delete;									// Prevent move-construction
		SceneSubsystem& operator=(SceneSubsystem&&) = delete;						// Prevent move-assignment

		std::string_view Name() const noexcept override;							//
		bool Initialize(EngineContext& context) override;							//
		void Shutdown() noexcept override;											//
		void Update(float deltaTime) override;										//

		void UnloadAllScenes();														//

		void AddScene(std::unique_ptr<Scene> pScene);								//
		void RemoveScene(Scene* pScene);											//

		Scene* GetCurrentScene() const;												//
		void SetCurrentScene(Scene* pScene);										//
		int GetSceneIndex(Scene* pScene) const;										//
	};
}