/*------------------------------
| File: Capsule.cpp
| Author: Chandler Mays
------------------------------*/
#include "Capsule.h"

#include "Systems/Rendering/Renderer.h"
#include "Utilities/Geometry/Rect.h"

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
void CaledonEngine::Capsule::Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color, float rotation, const Vector2f& origin) const
{
	if (!pRenderer)
		return;

	const float centerX = destRect.m_x + destRect.m_width / 2.0f;
	const float centerY = destRect.m_y + destRect.m_height / 2.0f;

	// A capsule is a line segment (a to b) swept by a radius: the segment runs along the longer side, between the two cap centers
	Vector2f a;
	Vector2f b;
	float radius;

	if (destRect.m_height > destRect.m_width)
	{
		radius = destRect.m_width / 2.0f;
		const float halfLength = (destRect.m_height - destRect.m_width) / 2.0f;
		a = Vector2f{ centerX, centerY - halfLength };
		b = Vector2f{ centerX, centerY + halfLength };
	}
	else
	{
		radius = destRect.m_height / 2.0f;
		const float halfLength = (destRect.m_width - destRect.m_height) / 2.0f;
		a = Vector2f{ centerX - halfLength, centerY };
		b = Vector2f{ centerX + halfLength, centerY };
	}

	a = RotateAround(a, origin, rotation);
	b = RotateAround(b, origin, rotation);

	pRenderer->DrawCapsule(a, b, radius, color, m_isFilled);
}