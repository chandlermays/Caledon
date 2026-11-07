/*------------------------------
| File: ShapeDistance2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/ShapeQueries2D.h"
#include "Systems/Physics/Types/ColliderDistance2D.h"
#include "Utilities/Math/Vector2.h"

// Pure "how far apart are these two shapes" for shapes that are already in world space (see Collider2D::GetShapes)
//
// The answer is written into the same ColliderDistance2D that Collider2D::Distance returns: it only holds two points, a normal and a distance,
// so it doesn't need to know about colliders. This file knows nothing about colliders, bodies or scenes
//
// Supported shapes: the ones ShapeQueries2D supports (circle, convex polygon, capsule, edges segment), each with its rounding radius
//
// Conventions (the same as Unity's ColliderDistance2D)
// * pointA is on the surface of shape A, pointB on the surface of shape B
// * distance > 0: separated, the gap between the two surfaces.  distance == 0: touching.  distance < 0: overlapping, by that much
// * normal is a unit vector that points from pointB toward pointA
// * pointA - pointB == normal * distance, in all three cases. So moving A by (-normal * distance) leaves the shapes just touching
//   (toward B when separated, out of B when overlapping: the shortest way out)
//
// How: if the rounded shapes overlap or touch, the contact manifold (ShapeManifold2D) already knows the shortest way out. If they are
// apart, the closest points of the "cores" (the polygon without its rounding, a segment, or the center of a circle) are found by comparing
// every edge of one with every edge of the other, and each radius is then added along the line between those two points
//
// References
// * Christer Ericson, Real-Time Collision Detection, section 5.1.9 "Closest Points of Two Line Segments" (the segment pair routine)

namespace CaledonEngine
{
	namespace ShapeDistance2D
	{
		bool Compute(const ShapeView2D& shapeA, const ShapeView2D& shapeB, ColliderDistance2D& result);					// Computes the closest points and distance between two shapes, returning true if the shapes are touching or overlapping
	}
}
