/*------------------------------
| File: BodySlide2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Queries/ColliderHit2D.h"
#include "Systems/Physics/Types/SlideResults2D.h"
#include "Utilities/Math/Vector2.h"

#include <functional>

// ...
// References
// * Quake style "collide and slide": Kasper Fauerby, "Improved Collision detection and Response" (2003) - project the leftover movement on the hit plane and repeat

namespace CaledonEngine
{
	using SlideCastFunction = std::function<bool(const Vector2f& position, const Vector2f& direction,				// The function to cast a collider in a direction, returning true if it hit something
		float distance, ColliderHit2D& hit)>;

	struct SlideInput2D
	{
		Vector2f position{ };																						// Where the body starts
		Vector2f velocity{ };																						// The velocity of the movement, in units per second
		float deltaTime = 0.0f;																						// The time the velocity is applied for
		Vector2f gravity{ };																						// Added as a final movement (gravity * deltaTime)
		float gravitySlipAngle = 90.0f;																				// The body slips from a gravity contact on surfaces steeper than this (degrees), if no movement was requested
		int maxIterations = 3;																						// How many casts a slide may use
		Vector2f surfaceAnchor{ };																					// The direction and distance to look for a surface nearby, after the movement
		float surfaceSlideAngle = 90.0f;																			// A surface shallower than this (degrees) lets the movement slide along it
		Vector2f surfaceUp{ 0.0f, 1.0f };																			// The direction angles are measured against
		float skin = 0.005f;																						// The gap left between the body and a surface it stops at
	};

	namespace BodySlide2D
	{
		void Compute(const SlideInput2D& input, const SlideCastFunction& cast, SlideResults2D& results);			// Computes the final position and remaining velocity of a body after sliding along surfaces
	}
}
