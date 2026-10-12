/*------------------------------
| File: Capsule.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Rendering/Shapes/Shape.h"

namespace CaledonEngine
{
	class Capsule : public Shape
	{
	private:

	public:
		Capsule(const Color& color = Color::White(), int width = 100, int height = 200, bool isFilled = true);			// Constructor
		~Capsule() override = default;																					// Destructor

		void Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color) const override;					// Draws the capsule onto the screen
	};
}