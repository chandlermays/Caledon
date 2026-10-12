/*------------------------------
| File: Triangle.cpp
| Author: Chandler Mays
------------------------------*/
#include "Triangle.h"

#include "Systems/Rendering/Renderer.h"
#include "Utilities/Math/Rect.h"
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
void CaledonEngine::Triangle::Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color) const
{
	if (!pRenderer)
		return;

	Vector2f v1(destRect.m_x + destRect.m_width / 2, destRect.m_y);
	Vector2f v2(destRect.m_x, destRect.m_y + destRect.m_height);
	Vector2f v3(destRect.m_x + destRect.m_width, destRect.m_y + destRect.m_height);

	pRenderer->DrawTriangle(v1, v2, v3, color, m_isFilled);
}