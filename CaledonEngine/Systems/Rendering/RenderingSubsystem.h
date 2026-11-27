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

	/*----------------------------------------------------------------------------------
	| --- RenderingSubsystem: Draws the current Scene through the platform         --- |
	| ---                     renderer. Each frame it gathers every active sprite  --- |
	| ---                     (skipping inactive GameObjects and any waiting to be --- |
	| ---                     destroyed, with their children), sorts them by layer --- |
	| ---                     and draws them in one pass. Sprites with the same    --- |
	| ---                     layer keep their Scene order                         --- |
	| ---                                                                          --- |
	| ---                     Drawing goes to the window, or to a render target    --- |
	| ---                     Texture when one is set (e.g. the Editor's Viewport) --- |
	----------------------------------------------------------------------------------*/
	class RenderingSubsystem final : public ISubsystem, public IRenderable
	{
	private:
		std::unique_ptr<Renderer> m_pRendererAPI;								// The Platform Backend Renderer
		EngineContext* m_pContext;												// Non-owning pointer to the EngineContext, set in Initialize
		SceneSubsystem* m_pScenes;												// Non-owning pointer to the SceneSubsystem, looked up on first Render
		Texture* m_pRenderTarget;												// Non-owning pointer to the Texture to draw into (nullptr = the window)
		std::vector<const SpriteComponent*> m_drawList;							// The sprites to draw this frame, sorted by layer

		void CacheSprites(const GameObject& gameObject);						// Adds a GameObject's drawable sprites (and its children's) to the draw list
		void RenderSprite(const SpriteComponent& sprite);						// Draws one sprite at its owner's world position, rotation and scale

	public:
		static constexpr std::string_view kName = "Rendering";					// Unique name used for registration and lookup

		RenderingSubsystem();													// Constructor
		~RenderingSubsystem() override;											// Destructor

		std::string_view Name() const noexcept override;						// Returns the Subsystem's name
		bool Initialize(EngineContext& context) override;						// Starts the platform renderer on the open window
		void Shutdown() noexcept override;										// Shuts down the platform renderer

		void BeginFrame() override;												// Begins the renderer's frame
		void Render() override;													// Draws the current Scene's sprites in layer order
		void EndFrame() override;												// Presents the frame

		Texture* GetRenderTarget() const noexcept;								// Returns the Texture being drawn into, or nullptr for the window
		void SetRenderTarget(Texture* pTarget);									// Sets the Texture to draw into (nullptr = the window)

		Renderer* GetRenderer() const noexcept;									// Returns the platform renderer
	};
}
