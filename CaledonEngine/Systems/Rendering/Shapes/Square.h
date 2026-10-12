/*------------------------------
| File: Square.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Rendering/Shapes/Shape.h"

namespace CaledonEngine
{
	class Square : public Shape
	{
	public:
		Square(const Color& color = Color::White(), int size = 100, bool isFilled = true);					// Constructor
		~Square() override = default;																		// Destructor

		void Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color) const override;		// Draws the square onto the screen
	};
}