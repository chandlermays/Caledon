/*------------------------------
| File: Circle.cpp
| Author: Chandler Mays
------------------------------*/
#include "Circle.h"

#include "Systems/Rendering/Renderer.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*--------------------------------------------------------------------
| --- Constructor: Constructs a Circle shape with default values --- |
--------------------------------------------------------------------*/
CaledonEngine::Circle::Circle(const Color& color, int radius, bool isFilled)
	: Shape(ShapeType::kCircle, color, radius * 2, radius * 2, isFilled)
	, m_radius{ radius }
{ }

/*------------------------------------------------------------------------------
| --- Render: Draws the circle onto the screen using the provided Renderer --- |
------------------------------------------------------------------------------*/
void CaledonEngine::Circle::Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color) const
{
	if (pRenderer == nullptr)
		return;

	// Calculate the center position of the circle
	float centerX = destRect.m_x + destRect.m_width / 2.0f;
	float centerY = destRect.m_y + destRect.m_height / 2.0f;

	// Calculate the radius based on the destination rectangle
	float scaledRadius = (destRect.m_width < destRect.m_height ? destRect.m_width : destRect.m_height) / 2.0f;

	pRenderer->DrawCircle(centerX, centerY, scaledRadius, color, m_isFilled);
}

/*--------------------------------------------------
| --- SetRadius: Sets the radius of the circle --- |
--------------------------------------------------*/
void CaledonEngine::Circle::SetRadius(int radius)
{
	m_radius = radius;
	m_width = radius * 2;
	m_height = radius * 2;
	m_bounds = Rect{ m_bounds.m_x, m_bounds.m_y, radius * 2, radius * 2 };
}

/*-----------------------------------------------------
| --- GetRadius: Returns the radius of the circle --- |
-----------------------------------------------------*/
int CaledonEngine::Circle::GetRadius() const
{
	return m_radius;
}