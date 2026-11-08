/*------------------------------
| File: Triangle.cpp
| Author: Chandler Mays
------------------------------*/
#include "Triangle.h"

#include "Systems/Rendering/Renderer.h"
#include "Utilities/Geometry/Rect.h"
#include "Utilities/Math/Vector2.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------
| --- Constructor: Constructs a Triangle shape with default values --- |
----------------------------------------------------------------------*/
CaledonEngine::Triangle::Triangle(const Color& color, int width, int height, bool isFilled)
	: Shape{ ShapeType::kTriangle, color, width, height, isFilled }
{ }

/*--------------------------------------------------------------------------------
| --- Render: Draws the triangle onto the screen using the provided Renderer --- |
--------------------------------------------------------------------------------*/
void CaledonEngine::Triangle::Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color, float rotation, const Vector2f& origin) const
{
	if (!pRenderer)
		return;

	Vector2f v1 = RotateAround({ destRect.m_x + destRect.m_width / 2.0f, destRect.m_y }, origin, rotation);
	Vector2f v2 = RotateAround({ destRect.m_x, destRect.m_y + destRect.m_height }, origin, rotation);
	Vector2f v3 = RotateAround({ destRect.m_x + destRect.m_width, destRect.m_y + destRect.m_height }, origin, rotation);

	pRenderer->DrawTriangle(v1, v2, v3, color, m_isFilled);
}