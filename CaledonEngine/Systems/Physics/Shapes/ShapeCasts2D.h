/*------------------------------
| File: ShapeCasts2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/ShapeQueries2D.h"
#include "Utilities/Math/Vector2.h"

// Pure shape cast: slide one shape along a straight line and find the first moment it touches another shape
// Both shapes are already in world space (see Collider2D::GetShapes). This file knows nothing about colliders, bodies, layers or scenes
//
// Supported shapes: the ones ShapeQueries2D supports (circle, convex polygon, capsule, edges segment), each with its rounding radius
//
// Conventions (the same as ColliderHit2D)
// * distance is how far the moving shape traveled along the direction when it first touched the target (0 = it already touched or overlapped at the start)
// * point is the contact point on the surface of the target
// * normal is the unit surface normal of the target at that point (it points away from the target, toward the moving shape).
//   For a shape that already overlaps at the start the normal is the opposite of the direction, like ShapeQueries2D::Raycast
// * The moving shape only slides, it never rotates
//
// How: a shape A moved by 's' touches a shape B exactly when 's' lies inside the "Minkowski difference" of B and A (all the points b - a).
// So casting A along a line is the same as casting a single RAY from the origin against that difference. For convex shapes the
// difference is the convex hull of every vertex of B minus every vertex of A, grown by the two radii. That hull is itself a polygon
// (or a segment, or a point), so the ray routine in ShapeQueries2D does the rest. A circle cast is the special case of one vertex
//
// References
// * Christer Ericson, Real-Time Collision Detection, section 3.9.1 "Computing Convex Hulls: Andrew's Algorithm"

namespace CaledonEngine
{
	struct ShapeCastHit2D
	{
		float distance = 0.0f;																						// How far the moving shape traveled before touching
		Vector2f point{ };																							// The contact point on the target's surface
		Vector2f normal{ };																							// The unit normal of the target's surface at the contact point
	};

	namespace ShapeCasts2D
	{
		bool Cast(const ShapeView2D& moving, const Vector2f& direction, float maxDistance,							// Casts the moving shape along the given direction and distance and returns true if it touches the target shape
			const ShapeView2D& target, bool hitIfStartsInside, ShapeCastHit2D& hit);
	}
}
