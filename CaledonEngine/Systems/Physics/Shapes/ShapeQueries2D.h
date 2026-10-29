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
// Capsule and Edges shapes are not handled yet
//
// Conventions
// * A shape includes its boundary: touching counts as overlapping
// * "Perimeter" means the outer surface of the shape, including the rounding added by the radius

namespace CaledonEngine
{
	class PhysicsShapeGroup2D;

	struct ShapeView2D																								//
	{
		PhysicsShapeType2D type = PhysicsShapeType2D::Circle;														//
		std::span<const Vector2f> vertices{ };																		//
		float radius = 0.0f;																						//
	};

	struct ShapeRayHit2D																							//
	{
		float distance = 0.0f;																						//
		Vector2f point{ };																							//
		Vector2f normal{ };																							//
	};

	namespace ShapeQueries2D
	{
		ShapeView2D MakeView(const PhysicsShapeGroup2D& shapeGroup, int shapeIndex);								//
		bool IsConvexCounterClockwise(std::span<const Vector2f> vertices);											//
		bool IsSupported(const ShapeView2D& shape);																	//

		bool OverlapPoint(const ShapeView2D& shape, const Vector2f& point);											//
		bool ClosestPointOnPerimeter(const ShapeView2D& shape, const Vector2f& point, Vector2f& result);			//

		bool Raycast(const ShapeView2D& shape, const Vector2f& origin, const Vector2f& direction,
			float maxDistance, bool hitIfStartsInside, ShapeRayHit2D& hit);											//

		bool Overlap(const ShapeView2D& shapeA, const ShapeView2D& shapeB);											//
	}
}