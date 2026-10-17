/*------------------------------
| File: SlideConfig2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Math/Vector2.h"

// The configuration that controls how RigidBody2D::Slide behaves

namespace CaledonEngine
{
	class Collider2D;

	struct SlideConfig2D
	{
		Vector2f gravity;								// The gravity to be applied to the slide position
		float gravitySlipAngle;							// When gravity movement causes a contact with a Collider2D, slippage may occur if the surface angle is greater than this angle
		int maxIterations;								// Controls the maximum number of iterations to perform when determining how a RigidBody2D will slide
		Collider2D* selectedCollider;					// The specific Collider2D attached to this RigidBody2D used to detect contacts
		Vector2f startPosition;							// The start position to slide the RigidBody2D from

		/*-----------------------------------------------------------------------
		| --- Constructor: Constructs the SlideConfig2D with default values --- |
		-----------------------------------------------------------------------*/
		constexpr SlideConfig2D()
			: gravity{}
			, gravitySlipAngle{ 0.0f }
			, maxIterations{ 1 }
			, selectedCollider{ nullptr }
			, startPosition{}
		{}
	};
}