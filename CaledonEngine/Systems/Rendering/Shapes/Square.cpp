/*------------------------------
| File: Square.cpp
| Author: Chandler Mays
------------------------------*/
#include "Square.h"

#include "Systems/Rendering/Renderer.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*--------------------------------------------------------------------
| --- Constructor: Constructs a Square shape with default values --- |
--------------------------------------------------------------------*/
CaledonEngine::Square::Square(const Color& color, int size, bool isFilled)
    : Shape{ ShapeType::kSquare, color, size, size, isFilled }
{ }

/*------------------------------------------------------------------------------
| --- Render: Draws the square onto the screen using the provided Renderer --- |
------------------------------------------------------------------------------*/
void CaledonEngine::Square::Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color, float rotation, const Vector2f& origin) const
{
    if (!pRenderer)
        return;

    Vector2f corners[4]
    {
        { destRect.m_x,                    destRect.m_y },
        { destRect.m_x + destRect.m_width, destRect.m_y },
        { destRect.m_x + destRect.m_width, destRect.m_y + destRect.m_height },
        { destRect.m_x,                    destRect.m_y + destRect.m_height }
    };

    for (Vector2f& corner : corners)
    {
        corner = RotateAround(corner, origin, rotation);
    }

    pRenderer->DrawQuad(corners[0], corners[1], corners[2], corners[3], color, m_isFilled);
}