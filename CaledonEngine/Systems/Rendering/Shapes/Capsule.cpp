/*------------------------------
| File: Capsule.cpp
| Author: Chandler Mays
------------------------------*/
#include "Capsule.h"

#include "Systems/Rendering/Renderer.h"
#include "Utilities/Math/Rect.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------
| --- Constructor: Constructs a Capsule shape with default values --- |
---------------------------------------------------------------------*/
CaledonEngine::Capsule::Capsule(const Color& color, int width, int height, bool isFilled)
	: Shape{ ShapeType::kCapsule, color, width, height, isFilled }
{ }

/*-------------------------------------------------------------------------------
| --- Render: Draws the capsule onto the screen using the provided Renderer --- |
-------------------------------------------------------------------------------*/
void CaledonEngine::Capsule::Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color) const
{
	if (!pRenderer)
		return;

	float centerX = destRect.m_x + destRect.m_width / 2.0f;
	float centerY = destRect.m_y + destRect.m_height / 2.0f;

	pRenderer->DrawCapsule(centerX, centerY, destRect.m_width, destRect.m_height, color, m_isFilled);
}