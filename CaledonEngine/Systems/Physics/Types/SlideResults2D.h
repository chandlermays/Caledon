/*------------------------------
| File: SlideResults2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Math/Vector2.h"

// The results of a slide movement performed with RigidBody2D.Slide
// These results can be used to both tune movement configuration and to implemnet further logic to react to the specific surfaces encountered when a slide occurs

namespace CaledonEngine
{
	struct SlideResults2D
	{
		int iterationsUsed;								// The number of iterations used when performing a Slide
		Vector2f position;								// The position that was calculated as a target position to move to when performing a Slide
		Vector2f remainingVelocity;						// The remaining velocity that couldn't be used when performing a Slide
		// Struct slideHit									// when a slide along a surface occurs, the slide may hit a surface tangent to the movement
		// Struct surfaceHit								// if the movement anchors to a surface or if graviy causes a contact with the surface

		/*------------------------------------------------------------------------
		| --- Constructor: Constructs the SlideResults2D with default values --- |
		------------------------------------------------------------------------*/
		constexpr SlideResults2D()
			: iterationsUsed{ 0 }
			, position{}
			, remainingVelocity{}
		{}
	};
}