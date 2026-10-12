/*------------------------------
| File: Circle.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Rendering/Shapes/Shape.h"

namespace CaledonEngine
{
	class Renderer;

	class Circle : public Shape
	{
	private:
		int m_radius;																						// The radius of the circle

	public:
		Circle(const Color& color = Color::White(), int radius = 50, bool isFilled = true);					// Constructor

		void Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color) const override;		// Draws the circle onto the screen

		void SetRadius(int radius);																			// Sets the radius of the circle
		int GetRadius() const;																				// Returns the radius of the circle
	};
}