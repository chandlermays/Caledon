/*------------------------------
| File: ShapeDistance2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ShapeDistance2D.h"

#include "Systems/Physics/Shapes/ShapeManifold2D.h"
#include "Utilities/Math/Constants.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace
{
	using namespace CaledonEngine;

	struct Segment
	{
		Vector2f a;
		Vector2f b;
	};

	/*----------------------------------------------------------------------------------------------------------------------------------------------
	| --- CoreSegments: Returns the segments of the "core" of a shape (the polygon without its rounding, a segment, or the center of a circle) --- |
	----------------------------------------------------------------------------------------------------------------------------------------------*/
	void CoreSegments(const ShapeView2D& shape, std::vector<Segment>& segments)
	{
		segments.clear();
		const std::size_t count = shape.vertices.size();
		if (shape.type == PhysicsShapeType2D::Circle)
		{
			segments.push_back({ shape.vertices[0], shape.vertices[0] });
		}
		else if (shape.type == PhysicsShapeType2D::Polygon)
		{
			for (std::size_t i = 0; i < count; ++i)
			{
				segments.push_back({ shape.vertices[i], shape.vertices[(i + 1 == count) ? 0 : i + 1] });
			}
		}
		else
		{
			segments.push_back({ shape.vertices[0], shape.vertices[1] });
		}
	}
}

/*----------------------------------------------------------------------------------------------------------------------------------------
| --- Compute: Computes the closest points and distance between two shapes, returning true if the shapes are touching or overlapping --- |
----------------------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeDistance2D::Compute(const ShapeView2D& shapeA, const ShapeView2D& shapeB, ColliderDistance2D& result)
{
	result = ColliderDistance2D{ };
	if (!ShapeQueries2D::IsSupported(shapeA) || !ShapeQueries2D::IsSupported(shapeB))
		return false;

	ShapeManifold2D manifold;
	if (ShapeManifold2DQueries::Collide(shapeA, shapeB, manifold) && manifold.pointCount > 0)
	{
		int deepest = 0;
		for (int i = 1; i < manifold.pointCount; ++i)
		{
			if (manifold.points[i].separation < manifold.points[deepest].separation)
				deepest = i;
		}

		const float separation = manifold.points[deepest].separation;
		const Vector2f& middle = manifold.points[deepest].point;
		const Vector2f halfOverlap = manifold.normal * (-0.5f * separation);
		result = ColliderDistance2D{ middle + halfOverlap, middle - halfOverlap, -manifold.normal, separation, separation < 0.0f };
		return true;
	}

	std::vector<Segment> coreA;
	std::vector<Segment> coreB;
	CoreSegments(shapeA, coreA);
	CoreSegments(shapeB, coreB);

	float bestSqrDistance = Math::MaxFloat;
	Vector2f closestA{ };
	Vector2f closestB{ };
	for (const Segment& first : coreA)
	{
		for (const Segment& second : coreB)
		{
			const SegmentClosestPoints2D pair = ShapeQueries2D::ClosestPointsOfSegments(first.a, first.b, second.a, second.b);
			const Vector2f delta = pair.pointOnSecond - pair.pointOnFirst;
			const float sqrDistance = delta.SqrMagnitude();
			if (sqrDistance < bestSqrDistance)
			{
				bestSqrDistance = sqrDistance;
				closestA = pair.pointOnFirst;
				closestB = pair.pointOnSecond;
			}
		}
	}

	const float coreDistance = std::sqrt(bestSqrDistance);
	const Vector2f towardB = (coreDistance > Math::Epsilon) ? (closestB - closestA) / coreDistance : Vector2f::Right();
	const float distance = std::max(coreDistance - shapeA.radius - shapeB.radius, 0.0f);
	result = ColliderDistance2D{ closestA + towardB * shapeA.radius, closestB - towardB * shapeB.radius, -towardB, distance, false };
	return true;
}