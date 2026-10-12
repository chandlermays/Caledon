/*------------------------------
| File: SDLRenderer.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Rendering/Renderer.h"

struct SDL_Renderer;

namespace CaledonEngine
{
	class SDLRenderer : public Renderer
	{
	private:
		SDL_Renderer* m_pRenderer;																		// Pointer to the SDL Renderer

	public:
		SDLRenderer();																					// Constructor
		virtual ~SDLRenderer() override;																// Destructor

		virtual bool Initialize(Window* pWindow) override;												// Prepares the SDL Renderer for use
		virtual void Shutdown() noexcept override;														// Cleans up the SDL Renderer
		virtual void Clear() override;																	// Clears the current render target
		virtual void BeginFrame() override;																// Prepares the SDL Renderer for a new frame
		virtual void EndFrame() override;																// Finalizes the rendering process for the current frame

		virtual std::shared_ptr<Texture> CreateRenderTarget(int width, int height) override;			// Creates an off-screen render target with the specified dimensions
		virtual void SetRenderTarget(Texture* pTarget) override;										// Sets the current render target to the default backbuffer

		virtual void RenderTexture(Texture* pTexture, RectInt* pSrc, RectFloat* pDest) override;		// Renders a texture to the screen
		std::shared_ptr<Texture> CreateTexture(Image* pImage) override;									// Creates a Texture from a loaded Image

		virtual void SetTextureColorMod(Texture* pTexture, uint8_t r,
			uint8_t g, uint8_t b) override;																// Sets the color modulation for a texture

		virtual void SetTextureAlphaMod(Texture* pTexture, uint8_t a) override;							// Sets the alpha modulation for a texture

		virtual void DrawRect(const RectFloat& rect, const Color& color, bool filled) override;			// Draws a rectangle to the screen

		virtual void DrawCircle(float centerX, float centerY, float radius,
			const Color& color, bool filled) override;													// Draws a circle to the screen

		virtual void DrawTriangle(const Vector2f& v1, const Vector2f& v2,
			const Vector2f& v3, const Color& color, bool filled) override;								// Draws a triangle to the screen

		virtual void DrawCapsule(float centerX, float centerY, float width, float height,
			const Color& color, bool filled) override;													// Draws a capsule to the screen

		virtual void* GetNativeHandle() const override;													// Returns the native renderer handle
	};
}