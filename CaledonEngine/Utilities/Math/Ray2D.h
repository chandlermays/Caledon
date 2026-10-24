/*------------------------------
| File: Ray2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Math/Vector2.h"

// In 2D spaces, this is a line segment that extends from a point in space in a specified direction.

namespace CaledonEngine
{
	struct Ray2D
	{
		Vector2f m_direction;												// The direction of the ray in world space
		Vector2f m_origin;													// The starting point of the ray in world space

		Ray2D()																// Constructor
			: m_origin(Vector2f::Zero()),
			, m_direction(Vector2f::Up())
		{ }

		Ray2D(const Vector2f& origin, const Vector2f& direction)			// Parameterized Constructor
			: m_origin{ origin }
			, m_direction{ direction }
		{ }

		// public Vector2f GetPoint(float distance);						// Returns a point that lies in a given distance along a ray
		// public string ToString()											// Returns a formatted string for this ray
	};
}