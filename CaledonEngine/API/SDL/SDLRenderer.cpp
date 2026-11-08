/*------------------------------
| File: SDLRenderer.cpp
| Author: Chandler Mays
------------------------------*/
#include "SDLRenderer.h"

#include "Systems/Window/Window.h"
#include "Systems/Rendering/Resources/Texture.h"
#include "Systems/Rendering/Resources/Image.h"
#include "API/SDL/SDLTexture.h"

#include <SDL3/SDL.h>
#include <limits>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------
| --- Constructor: Constructs the 'SDL' Renderer with default values --- |
------------------------------------------------------------------------*/
CaledonEngine::SDLRenderer::SDLRenderer()
	: m_pRenderer{ nullptr }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::SDLRenderer::~SDLRenderer()
{
	Shutdown();
}

/*---------------------------------------------------------
| --- Initialize: Prepares the 'SDL' Renderer for use --- |
---------------------------------------------------------*/
bool CaledonEngine::SDLRenderer::Initialize(Window* pWindow)
{
	if (!pWindow)
		return false;

	SDL_Window* pSDLWindow = static_cast<SDL_Window*>(pWindow->GetNativeHandle());
	if (!pSDLWindow)
		return false;

	m_pRenderer = SDL_CreateRenderer(pSDLWindow, NULL);
	if (!m_pRenderer)
		return false;

	return true;
}

/*------------------------------------------------
| --- Shutdown: Cleans up the 'SDL' Renderer --- |
------------------------------------------------*/
void CaledonEngine::SDLRenderer::Shutdown() noexcept
{
	if (!m_pRenderer)
		return;

	SDL_DestroyRenderer(m_pRenderer);
	m_pRenderer = nullptr;
}

/*-------------------------------------------------
| --- Clear: Clears the current render target --- |
-------------------------------------------------*/
void CaledonEngine::SDLRenderer::Clear()
{
	if (!m_pRenderer)
		return;

	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer);
}

/*-----------------------------------------------------------------
| --- BeginFrame: Prepares the 'SDL' Renderer for a new frame --- |
-----------------------------------------------------------------*/
void CaledonEngine::SDLRenderer::BeginFrame()
{
	Clear();
}

/*-------------------------------------------------------------------------
| --- EndFrame: Finalizes the rendering process for the current frame --- |
-------------------------------------------------------------------------*/
void CaledonEngine::SDLRenderer::EndFrame()
{
	if (!m_pRenderer)
		return;

	SDL_RenderPresent(m_pRenderer);
}

/*-----------------------------------------------------------------------------------------------
| --- CreateRenderTarget: Creates an off-screen render target with the specified dimensions --- |
-----------------------------------------------------------------------------------------------*/
std::shared_ptr<CaledonEngine::Texture> CaledonEngine::SDLRenderer::CreateRenderTarget(int width, int height)
{
	if (!m_pRenderer || width <= 0 || height <= 0)
		return nullptr;

	SDL_Texture* pSDLTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	if (!pSDLTexture)
		return nullptr;

	auto pTexture = std::make_shared<SDLTexture>();
	if (!pTexture->Load(pSDLTexture, width, height))
	{
		SDL_DestroyTexture(pSDLTexture);
		return nullptr;
	}

	return pTexture;
}

/*-----------------------------------------------------------------------------------
| --- SetRenderTarget: Sets the current render target to the default backbuffer --- |
-----------------------------------------------------------------------------------*/
void CaledonEngine::SDLRenderer::SetRenderTarget(Texture* pTarget)
{
	if (!m_pRenderer)
		return;

	SDL_Texture* pSDLTexture = pTarget ? static_cast<SDL_Texture*>(pTarget->GetNativeHandle()) : nullptr;
	SDL_SetRenderTarget(m_pRenderer, pSDLTexture);
}

/*-----------------------------------------------------
| --- RenderCopy: Renders a texture to the screen --- |
-----------------------------------------------------*/
void CaledonEngine::SDLRenderer::RenderTexture(Texture* pTexture, RectInt* pSrc, RectFloat* pDest)
{
	if (!m_pRenderer || !pTexture || !pDest)
		return;

	SDL_Texture* pSDLTexture = static_cast<SDL_Texture*>(pTexture->GetNativeHandle());
	SDL_FRect srcRect;
	SDL_FRect destRect;
	if (pSrc)
	{
		srcRect.x = static_cast<float>(pSrc->m_x);
		srcRect.y = static_cast<float>(pSrc->m_y);
		srcRect.w = static_cast<float>(pSrc->m_width);
		srcRect.h = static_cast<float>(pSrc->m_height);
	}

	destRect.x = pDest->m_x;
	destRect.y = pDest->m_y;
	destRect.w = pDest->m_width;
	destRect.h = pDest->m_height;

	SDL_RenderTexture(m_pRenderer, pSDLTexture, pSrc ? &srcRect : nullptr, &destRect);
}

/*--------------------------------------------------------------
| --- CreateTexture: Creates a Texture from a loaded Image --- |
--------------------------------------------------------------*/
std::shared_ptr<CaledonEngine::Texture> CaledonEngine::SDLRenderer::CreateTexture(Image* pImage)
{
	if (!m_pRenderer || !pImage)
		return nullptr;

	SDL_Surface* pSurface = static_cast<SDL_Surface*>(pImage->GetNativeHandle());
	if (!pSurface)
		return nullptr;

	SDL_Texture* pSDLTexture = SDL_CreateTextureFromSurface(m_pRenderer, pSurface);
	if (!pSDLTexture)
		return nullptr;

	auto pTexture = std::make_shared<SDLTexture>();
	if (!pTexture->Load(pSDLTexture, pSurface->w, pSurface->h))
	{
		SDL_DestroyTexture(pSDLTexture);
		return nullptr;
	}

	return pTexture;
}

/*---------------------------------------------------------------------
| --- SetTextureColorMod: Sets the color modulation for a texture --- |
---------------------------------------------------------------------*/
void CaledonEngine::SDLRenderer::SetTextureColorMod(Texture* pTexture, uint8_t r, uint8_t g, uint8_t b)
{
	if (!m_pRenderer || !pTexture)
		return;

	SDL_Texture* pSDLTexture = static_cast<SDL_Texture*>(pTexture->GetNativeHandle());
	SDL_SetTextureColorMod(pSDLTexture, r, g, b);
}

/*---------------------------------------------------------------------
| --- SetTextureAlphaMod: Sets the alpha modulation for a texture --- |
---------------------------------------------------------------------*/
void CaledonEngine::SDLRenderer::SetTextureAlphaMod(Texture* pTexture, uint8_t a)
{
	if (!m_pRenderer || !pTexture)
		return;

	SDL_Texture* pSDLTexture = static_cast<SDL_Texture*>(pTexture->GetNativeHandle());
	SDL_SetTextureAlphaMod(pSDLTexture, a);
}


/*---------------------------------------------------
| --- DrawRect: Draws a rectangle to the screen --- |
---------------------------------------------------*/
void CaledonEngine::SDLRenderer::DrawRect(const RectFloat& rect, const Color& color, bool filled)
{
	if (!m_pRenderer)
		return;

	SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);

	SDL_FRect sdlRect
	{
		rect.m_x,
		rect.m_y,
		rect.m_width,
		rect.m_height
	};

	if (filled)
	{
		SDL_RenderFillRect(m_pRenderer, &sdlRect);
	}
	else
	{
		SDL_RenderRect(m_pRenderer, &sdlRect);
	}
}

/*--------------------------------------------------
| --- DrawCircle: Draws a circle to the screen --- |
--------------------------------------------------*/
void CaledonEngine::SDLRenderer::DrawCircle(float centerX, float centerY, float radius, const Color& color, bool filled)
{
	if (!m_pRenderer)
		return;

	SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);

	float x = radius;
	float y = 0;
	float err = 0;

	if (filled)
	{
		// Draw filled circle
		while (x >= y)
		{
			// Draw horizontal lines to fill the circle
			SDL_RenderLine(m_pRenderer, centerX - x, centerY + y, centerX + x, centerY + y);
			SDL_RenderLine(m_pRenderer, centerX - y, centerY + x, centerX + y, centerY + x);
			SDL_RenderLine(m_pRenderer, centerX - x, centerY - y, centerX + x, centerY - y);
			SDL_RenderLine(m_pRenderer, centerX - y, centerY - x, centerX + y, centerY - x);

			if (err <= 0)
			{
				y += 1;
				err += 2 * y + 1;
			}

			if (err > 0)
			{
				x -= 1;
				err -= 2 * x + 1;
			}
		}
	}
	else
	{
		// Draw outlined circle
		while (x >= y)
		{
			// Draw 8 symmetric points
			SDL_RenderPoint(m_pRenderer, centerX + x, centerY + y);
			SDL_RenderPoint(m_pRenderer, centerX + y, centerY + x);
			SDL_RenderPoint(m_pRenderer, centerX - y, centerY + x);
			SDL_RenderPoint(m_pRenderer, centerX - x, centerY + y);
			SDL_RenderPoint(m_pRenderer, centerX - x, centerY - y);
			SDL_RenderPoint(m_pRenderer, centerX - y, centerY - x);
			SDL_RenderPoint(m_pRenderer, centerX + y, centerY - x);
			SDL_RenderPoint(m_pRenderer, centerX + x, centerY - y);

			if (err <= 0)
			{
				y += 1;
				err += 2 * y + 1;
			}

			if (err > 0)
			{
				x -= 1;
				err -= 2 * x + 1;
			}
		}
	}
}

/*------------------------------------------------------
| --- DrawTriangle: Draws a triangle to the screen --- |
------------------------------------------------------*/
void CaledonEngine::SDLRenderer::DrawTriangle(const Vector2f& v1, const Vector2f& v2, const Vector2f& v3, const Color& color, bool filled)
{
	if (!m_pRenderer)
		return;

	SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);

	if (filled)
	{
		int minY = static_cast<int>(std::min({ v1.y, v2.y, v3.y }));
		int maxY = static_cast<int>(std::max({ v1.y, v2.y, v3.y }));

		for (int y = minY; y <= maxY; ++y)
		{
			float fy = static_cast<float>(y);
			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::lowest();
			bool foundEdge = false;

			auto checkEdge = [&](const Vector2f& p1, const Vector2f& p2)
				{
					if ((p1.y <= fy && p2.y >= fy) || (p2.y <= fy && p1.y >= fy))
					{
						if (p2.y != p1.y)
						{
							float x = p1.x + (fy - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
							minX = std::min(minX, x);
							maxX = std::max(maxX, x);
							foundEdge = true;
						}
					}
				};

			checkEdge(v1, v2);
			checkEdge(v2, v3);
			checkEdge(v3, v1);

			if (foundEdge)
			{
				SDL_RenderLine(m_pRenderer, minX, fy, maxX, fy);
			}
		}
	}
	else
	{
		SDL_RenderLine(m_pRenderer, v1.x, v1.y, v2.x, v2.y);
		SDL_RenderLine(m_pRenderer, v2.x, v2.y, v3.x, v3.y);
		SDL_RenderLine(m_pRenderer, v3.x, v3.y, v1.x, v1.y);
	}
}

/*---------------------------------------------
| --- DrawQuad: Draws a quad to the screen --- |
---------------------------------------------*/
void CaledonEngine::SDLRenderer::DrawQuad(const Vector2f& v1, const Vector2f& v2, const Vector2f& v3, const Vector2f& v4, const Color& color, bool filled)
{
	if (!m_pRenderer)
		return;

	if (filled)
	{
		// Split the quad along its diagonal into two triangles
		DrawTriangle(v1, v2, v3, color, true);
		DrawTriangle(v1, v3, v4, color, true);
	}
	else
	{
		SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderLine(m_pRenderer, v1.x, v1.y, v2.x, v2.y);
		SDL_RenderLine(m_pRenderer, v2.x, v2.y, v3.x, v3.y);
		SDL_RenderLine(m_pRenderer, v3.x, v3.y, v4.x, v4.y);
		SDL_RenderLine(m_pRenderer, v4.x, v4.y, v1.x, v1.y);
	}
}

/*----------------------------------------------------
| --- DrawCapsule: Draws a capsule to the screen --- |
----------------------------------------------------*/
void CaledonEngine::SDLRenderer::DrawCapsule(const Vector2f& a, const Vector2f& b, float radius, const Color& color, bool filled)
{
	if (!m_pRenderer)
		return;

	// Cap at each end of the segment
	DrawCircle(a.x, a.y, radius, color, filled);
	DrawCircle(b.x, b.y, radius, color, filled);

	// Body between the caps: offset the segment sideways by the radius (perpendicular to its direction)
	const Vector2f side = (b - a).Normalized().Perpendicular() * radius;
	if (side.SqrMagnitude() == 0.0f)
		return;																		// Both caps sit on the same point, so the capsule is just a circle

	if (filled)
	{
		DrawQuad(a + side, b + side, b - side, a - side, color, true);
	}
	else
	{
		SDL_SetRenderDrawColor(m_pRenderer, color.r, color.g, color.b, color.a);
		SDL_RenderLine(m_pRenderer, a.x + side.x, a.y + side.y, b.x + side.x, b.y + side.y);
		SDL_RenderLine(m_pRenderer, a.x - side.x, a.y - side.y, b.x - side.x, b.y - side.y);
	}
}

/*-------------------------------------------------------------
| --- GetNativeHandle: Returns the native renderer handle --- |
-------------------------------------------------------------*/
void* CaledonEngine::SDLRenderer::GetNativeHandle() const
{
	return static_cast<void*>(m_pRenderer);
}