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
	class Physics2DSubsystem;
	class Scene;

	/*---------------------------------------------------------------------------------------------
	| --- SceneSubsystem: Owns every Scene and tracks which one is current. Each frame it     --- |
	| ---                 updates only the current Scene (and only while it is active), but  --- |
	| ---                 flushes every Scene's queued adds and destructions, so the Editor's --- |
	| ---                 changes go through even when the Scene isn't being updated         --- |
	| ---                                                                                     --- |
	| ---                 Scenes can be added before or after the Subsystem is initialized:   --- |
	| ---                 ones added before are initialized in Initialize, ones added after   --- |
	| ---                 are initialized right away. Each Scene is handed the default 2D     --- |
	| ---                 Physics Scene, or nullptr when there is no Physics2DSubsystem       --- |
	| ---                                                                                     --- |
	| ---                 The first Scene added becomes current. Only an owned Scene can be   --- |
	| ---                 made current, and removing the current Scene leaves none current    --- |
	| ---                 (there is no automatic fallback to another Scene)                   --- |
	---------------------------------------------------------------------------------------------*/
	class SceneSubsystem final : public ISubsystem, public IUpdatable
	{
	private:
		std::vector<std::unique_ptr<Scene>> m_pScenes;								// Every Scene owned by the Subsystem
		Scene* m_pCurrentScene;														// Non-owning pointer to the Scene updated each frame
		EngineContext* m_pContext;													// Non-owning pointer to the EngineContext, set in Initialize
		Physics2DSubsystem* m_pPhysics2D;											// Non-owning pointer to the 2D Physics Subsystem (may be nullptr)

		bool InitializeScene(Scene& scene);											// Assigns 2D physics and initializes a Scene

	public:
		static constexpr std::string_view kName = "Scene";							// Unique name used for registration and lookup

		SceneSubsystem();															// Constructor
		~SceneSubsystem() override;													// Destructor
		SceneSubsystem(const SceneSubsystem&) = delete;								// Prevent copy-construction
		SceneSubsystem& operator=(const SceneSubsystem&) = delete;					// Prevent copy-assignment
		SceneSubsystem(SceneSubsystem&&) = delete;									// Prevent move-construction
		SceneSubsystem& operator=(SceneSubsystem&&) = delete;						// Prevent move-assignment

		std::string_view Name() const noexcept override;							// Returns the Subsystem's name
		bool Initialize(EngineContext& context) override;							// Caches dependencies and initializes owned Scenes
		void Shutdown() noexcept override;											// Unloads all Scenes and clears cached pointers
		void Update(float deltaTime) override;										// Updates the current Scene and flushes pending changes

		void UnloadAllScenes();														// Destroys every Scene

		bool AddScene(std::unique_ptr<Scene> pScene);								// Takes ownership of and initializes a Scene
		void RemoveScene(Scene* pScene);											// Destroys the given Scene

		Scene* GetCurrentScene() const;												// Returns the current Scene
		void SetCurrentScene(Scene* pScene);										// Sets the current Scene if it's owned
		int GetSceneIndex(Scene* pScene) const;										// Returns the Scene's index, or -1 if not found
	};
}
