/*------------------------------
| File: PhysicsShapeType2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// Options for indicate which primitive shape type is used to intepret geometry contained within a PhysicsShape2D object

namespace CaledonEngine
{
	enum class PhysicsShapeType2D : uint8_t
	{
		Circle,					// One vertex (the center) plus a radius
		Capsule,				// Two vertices (the centers of the two end circles) plus a radius
		Polygon,				// Three or more vertices forming a convex polygon (counter-clockwise); radius rounds its corners
		Edges,					// Two or more vertices forming an open chain of edges; radius thickens it

		// ..expand for Caledon's capabilities
	};
}