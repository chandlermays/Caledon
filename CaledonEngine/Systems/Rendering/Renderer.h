/*------------------------------
| File: Renderer.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Rendering/Color.h"
#include "Utilities/Geometry/Rect.h"
#include "Utilities/Math/Vector2.h"

#include <memory>

namespace CaledonEngine
{
	class Window;
	class Texture;
	class Image;

	class Renderer
	{
	protected:
		std::string m_lastError;																		// Error message for why the Initialize failed

	public:
		Renderer() = default;																			// Constructor
		virtual ~Renderer() = default;																	// Destructor
		Renderer(const Renderer&) = delete;																// Prevent copy-construction
		Renderer& operator=(const Renderer&) = delete;													// Prevent copy-assignment
		Renderer(Renderer&&) = delete;																	// Prevent move-construction
		Renderer& operator=(Renderer&&) = delete;														// Prevent move-assignment

		virtual bool Initialize(Window* pWindow) = 0;													// Prepares the Renderer for use
		virtual void Shutdown() noexcept = 0;															// Cleans up the Renderer
		virtual void Clear() = 0;																		// Clears the current render target
		virtual void BeginFrame() = 0;																	// Prepares the Renderer for a new frame
		virtual void EndFrame() = 0;																	// Finalizes the rendering process for the current frame

		virtual std::shared_ptr<Texture> CreateRenderTarget(int width, int height) = 0;					// Creates an off-screen render target
		virtual void SetRenderTarget(Texture* pTarget) = 0;												// Sets the current render target to the default backbuffer

		virtual void RenderTexture(Texture* pTexture, RectInt* pSrc, RectFloat* pDest) = 0;				// Renders a texture to the screen
		virtual std::shared_ptr<Texture> CreateTexture(Image* pImage) = 0;								// Creates a Texture from a loaded Image

		virtual void SetTextureColorMod(Texture* pTexture, uint8_t r,
			uint8_t g, uint8_t b) = 0;																	// Sets the color modulation for a texture

		virtual void SetTextureAlphaMod(Texture* pTexture, uint8_t a) = 0;								// Sets the alpha modulation for a texture

		virtual void DrawRect(const RectFloat& rect, const Color& color, bool filled) = 0;				// Draws an outlined rectangle

		virtual void DrawCircle(float centerX, float centerY, float radius,
			const Color& color, bool filled) = 0;														// Draws an outlined circle

		virtual void DrawTriangle(const Vector2f& v1, const Vector2f& v2,
			const Vector2f& v3, const Color& color, bool filled) = 0;									// Draws an outlined triangle

		virtual void DrawCapsule(float centerX, float centerY, float width, float height,
			const Color& color, bool filled) = 0;														// Draws an outlined capsule

		virtual void* GetNativeHandle() const = 0;														// Returns the native Renderer handle
		const std::string& GetLastError() const noexcept { return m_lastError; }
	};
}