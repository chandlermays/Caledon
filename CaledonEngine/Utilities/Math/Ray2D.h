/*------------------------------
| File: Ray2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Math/Vector2.h"

#include <string>

// In 2D spaces, this is a line segment that extends from a point in space in a specified direction.

namespace CaledonEngine
{
	struct Ray2D
	{
		Vector2f m_direction;												// The direction of the ray in world space
		Vector2f m_origin;													// The starting point of the ray in world space

		Ray2D()																// Constructor
			: m_origin(Vector2f::Zero())
			, m_direction(Vector2f::Up())
		{ }

		Ray2D(const Vector2f& origin, const Vector2f& direction)			// Parameterized Constructor
			: m_origin{ origin }
			, m_direction{ direction }
		{ }

		/*-----------------------------------------------------------------------------
		| --- GetPoint: Returns a point that lies at a given distance along a ray --- |
		-----------------------------------------------------------------------------*/
		constexpr Vector2f GetPoint(float distance) const noexcept
		{
			return m_origin + m_direction * distance;
		}

		/*-----------------------------------------------------------
		| --- ToString: Returns a formatted string for this ray --- |
		-----------------------------------------------------------*/
		std::string ToString() const
		{
			return "Ray2D(Origin: (" + std::to_string(m_origin.x) + ", " + std::to_string(m_origin.y) +
				"), Direction: (" + std::to_string(m_direction.x) + ", " + std::to_string(m_direction.y) + "))";
		}
	};
}