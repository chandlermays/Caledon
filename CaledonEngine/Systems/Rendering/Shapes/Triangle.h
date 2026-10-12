/*------------------------------
| File: Triangle.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Rendering/Shapes/Shape.h"

namespace CaledonEngine
{
	class Triangle : public Shape
	{
	public:
		Triangle(const Color& color = Color::White(), int width = 100, int height = 100, bool isFilled = true);			// Constructor
		~Triangle() override = default;																					// Destructor

		void Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color) const override;					// Draws the triangle onto the screen
	};
}