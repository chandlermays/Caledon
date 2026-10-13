/*------------------------------
| File: RenderingSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IRenderable.h"

#include <memory>
#include <string_view>
#include <vector>

namespace CaledonEngine
{
	class EngineContext;
	class GameObject;
	class Renderer;
	class SceneSubsystem;
	class SpriteComponent;
	class Texture;

	class RenderingSubsystem final : public ISubsystem, public IRenderable
	{
	private:
		std::unique_ptr<Renderer> m_pRendererAPI;								// The Platform Backend Renderer
		EngineContext* m_pContext;												//
		SceneSubsystem* m_pScenes;												//
		Texture* m_pRenderTarget;												//
		std::vector<const SpriteComponent*> m_drawList;							//

		void CacheSprites(const GameObject& gameObject);						//
		void RenderSprite(const SpriteComponent& sprite);						//

	public:
		static constexpr std::string_view kName = "Rendering";					//

		RenderingSubsystem();													// Constructor
		~RenderingSubsystem() override;											// Destructor

		std::string_view Name() const noexcept override;						//
		bool Initialize(EngineContext& context) override;						//
		void Shutdown() noexcept override;										//

		void BeginFrame() override;												//
		void Render() override;													//
		void EndFrame() override;												//

		Texture* GetRenderTarget() const noexcept;								//
		void SetRenderTarget(Texture* pTarget);									//

		Renderer* GetRenderer() const noexcept;									//
	};
}
