/*------------------------------
| File: SceneSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IUpdatable.h"
#include "Systems/Engine/IRenderable.h"

#include <memory>
#include <vector>

namespace CaledonEngine
{
	class EngineContext;
	class Scene;
	class Renderer;
	class Texture;

	class SceneSubsystem final : public ISubsystem, public IUpdatable, public IRenderable
	{
	private:
		std::vector<std::unique_ptr<Scene>> m_pScenes;								//
		Scene* m_pCurrentScene;														//

		Renderer* m_pRenderer;														//
		Texture* m_pRenderTarget;													//

	public:
		SceneSubsystem();															// Constructor
		~SceneSubsystem() override;													// Destructor
		SceneSubsystem(const SceneSubsystem&) = delete;								// Prevent copy-construction
		SceneSubsystem& operator=(const SceneSubsystem&) = delete;					// Prevent copy-assignment
		SceneSubsystem(SceneSubsystem&&) = delete;									// Prevent move-construction
		SceneSubsystem& operator=(SceneSubsystem&&) = delete;						// Prevent move-assignment

		bool Initialize(EngineContext& context) override;							//
		void Shutdown() noexcept override;											//
		void Update(float deltaTime) override;										//
		void Render() override;														//

		void UnloadAllScenes();														//

		void AddScene(std::unique_ptr<Scene> pScene);
		void RemoveScene(std::unique_ptr<Scene> pScene);

		Scene* GetCurrentScene() const;
		void SetCurrentScene(Scene* pScene);
		int GetSceneIndex(Scene* pScene) const;

		Texture* GetRenderTarget() const;
		void SetRenderTarget(Texture* pTexture);
	};
}