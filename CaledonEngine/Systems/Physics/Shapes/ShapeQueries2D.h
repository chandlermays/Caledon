/*------------------------------
| File: ShapeQueries2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/PhysicsShapeType2D.h"
#include "Utilities/Math/Vector2.h"

#include <span>

// Pure geometry queries on shapes that are ALREADY in world space (see Collider2D::GetShapes)
//
// These functions know nothing about colliders, scenes or layers. They only answer geometric questions about one or two shapes,
// which keeps them easy to test on their own.
//
// Supported shapes (everything else is reported as unsupported, see IsSupported):
// * Circle  : one vertex (the center) plus a radius
// * Polygon : three or more vertices forming a CONVEX, COUNTER-CLOCKWISE polygon. The radius rounds the corners (the polygon grows by the radius on every side)
// * Capsule : two distinct vertices (the centers of the two end circles) plus a radius. It is handled as a rounded polygon that has only two
//             corners: the "polygon" is the segment between the vertices, and the radius grows it into a capsule. Its two "edges" are the same
//             segment seen from both sides (opposite normals), so every routine that walks the edges of a polygon works for it as well
// * Edges   : exactly two distinct vertices (ONE segment) and a radius that may be zero. Handled like a capsule, so it is two-sided. A chain of
//             several edges is not convex, so the caller adds it as one Edges shape per segment (EdgeCollider2D does). Edges have no mass
//
// Conventions
// * A shape includes its boundary: touching counts as overlapping
// * "Perimeter" means the outer surface of the shape, including the rounding added by the radius

namespace CaledonEngine
{
	class PhysicsShapeGroup2D;

	struct SegmentClosestPoints2D																														// The closest pair of points on two segments
	{
		float s = 0.0f;																																	// Parameter of pointOnFirst along the first segment (0 at p1, 1 at q1)
		float t = 0.0f;																																	// Parameter of pointOnSecond along the second segment (0 at p2, 1 at q2)
		Vector2f pointOnFirst{ };																														// The closest point on the first segment (p1 -> q1)
		Vector2f pointOnSecond{ };																														// The closest point on the second segment (p2 -> q2)
	};

	struct ShapeView2D																																	// Lightweight read-only view of a physics shape for geometry queries
	{
		PhysicsShapeType2D type = PhysicsShapeType2D::Circle;																							// The type classification of the shape
		std::span<const Vector2f> vertices{ };																											// Vertex span representing the core geometry of the shape
		float radius = 0.0f;																															// Collision skin or rounding radius of the shape
	};

	struct ShapeRayHit2D																																// Result data returned when a ray intersects a shape view
	{
		float distance = 0.0f;																															// Distance along the ray where the intersection occurred
		Vector2f point{ };																																// World-space point where the ray hit the shape surface
		Vector2f normal{ };																																// Outward-facing surface normal at the hit point
	};

	namespace ShapeQueries2D
	{
		ShapeView2D MakeView(const PhysicsShapeGroup2D& shapeGroup, int shapeIndex);																	// Creates a ShapeView2D descriptor for a specific shape index within a shape group
		bool IsConvexCounterClockwise(std::span<const Vector2f> vertices);																				// Validates whether a given vertex collection forms a convex, counter-clockwise polygon
		bool IsSupported(const ShapeView2D& shape);																										// Checks if a shape view configuration is fully supported by the query pipeline

		bool OverlapPoint(const ShapeView2D& shape, const Vector2f& point);																				// Tests whether a world-space point overlaps the interior or boundary of a shape
		bool ClosestPointOnPerimeter(const ShapeView2D& shape, const Vector2f& point, Vector2f& result);												// Finds the point on the outer perimeter of a shape closest to a target reference position
		SegmentClosestPoints2D ClosestPointsOfSegments(const Vector2f& p1, const Vector2f& q1, const Vector2f& p2, const Vector2f& q2) noexcept;		// Finds the closest points on two line segments

		bool Raycast(const ShapeView2D& shape, const Vector2f& origin, const Vector2f& direction,
			float maxDistance, bool hitIfStartsInside, ShapeRayHit2D& hit);																				// Casts a ray against a shape view and populates hit details upon intersection

		bool Overlap(const ShapeView2D& shapeA, const ShapeView2D& shapeB);																				// Tests whether two shape views intersect or touch each other
	}
}