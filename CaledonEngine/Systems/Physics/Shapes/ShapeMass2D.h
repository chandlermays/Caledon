/*------------------------------
| File: ShapeMass2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/ShapeQueries2D.h"
#include "Utilities/Math/Vector2.h"

// Pure mass geometry for one shape, in whatever frame the shape's vertices are in (for a RigidBody2D that is the body's local frame)
//
// Everything is "per unit density": multiply by a density to get the real mass and inertia
// * area      : the shape's area. Multiply by density to get mass
// * centroid  : the shape's center of mass
// * inertia   : the area moment of inertia about the frame's ORIGIN (not the centroid). Multiply by density to get rotational inertia about the origin
//
// Supported shapes
// * Circle  : area = pi * r^2
// * Polygon : a convex polygon in either winding order. The rounding radius is ignored: it is a thin collision skin, not extra body
// Capsule and Edges shapes have no mass here yet (edges are massless by design, capsules arrive with capsule support)

namespace CaledonEngine
{
	struct ShapeMassData2D
	{
		float area = 0.0f;																			// The area of the shape
		Vector2f centroid{ };																		// The center of mass of the shape
		float inertia = 0.0f;																		// The area moment of inertia about the origin of the frame
	};

	namespace ShapeMass2D
	{
		bool Compute(const ShapeView2D& shape, ShapeMassData2D& massData);							// Computes the mass data of a shape; returns false if the shape has no area or isn't supported
	}
}
