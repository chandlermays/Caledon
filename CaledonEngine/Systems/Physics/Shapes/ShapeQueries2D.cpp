/*------------------------------
| File: ShapeQueries2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ShapeQueries2D.h"

#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"

#include <algorithm>
#include <cmath>
#include <limits>

/*------------------------------------
| --- Private Helper Definitions --- |
------------------------------------*/
namespace
{
	using namespace CaledonEngine;

	struct BoundaryPoint
	{
		Vector2f point{ };
		std::size_t edge = 0;
		float sqrDistance = Math::MaxFloat;
	};

	/*------------------------------------------------------------------------------------------------
	| --- NextIndex: Returns the next vertex index in a ring, wrapping around to zero at the end --- |
	------------------------------------------------------------------------------------------------*/
	inline std::size_t NextIndex(std::size_t i, std::size_t count)
	{
		return (i + 1 == count) ? 0 : i + 1;
	}

	/*------------------------------------------------------------------------------------------------------
	| --- OutwardNormal: Computes the normalized outward-facing normal vector for a given polygon edge --- |
	------------------------------------------------------------------------------------------------------*/
	inline Vector2f OutwardNormal(const Vector2f& a, const Vector2f& b)
	{
		const Vector2f edge = b - a;
		return Vector2f{ edge.y, -edge.x }.Normalized();
	}

	/*--------------------------------------------------------------------------------------------
	| --- ClosestPointOnSegment: Finds the closest point on a line segment to a target point --- |
	--------------------------------------------------------------------------------------------*/
	Vector2f ClosestPointOnSegment(const Vector2f& p, const Vector2f& a, const Vector2f& b)
	{
		const Vector2f ab = b - a;
		const float lengthSqr = Vector2f::Dot(ab, ab);
		if (lengthSqr <= 0.0f)
			return a;

		const float t = std::clamp(Vector2f::Dot(p - a, ab) / lengthSqr, 0.0f, 1.0f);
		return a + ab * t;
	}

	/*--------------------------------------------------------------------------------------------------------------
	| --- IsInsidePolygon: Tests whether a point lies strictly inside a convex polygon using half-plane checks --- |
	--------------------------------------------------------------------------------------------------------------*/
	bool IsInsidePolygon(std::span<const Vector2f> vertices, const Vector2f& p)
	{
		const std::size_t count = vertices.size();
		if (count < 3)
			return false;

		for (std::size_t i = 0; i < count; ++i)
		{
			const Vector2f& a = vertices[i];
			const Vector2f& b = vertices[NextIndex(i, count)];

			if (Vector2f::Cross(b - a, p - a) < 0.0f)
				return false;
		}
		return true;
	}

	/*----------------------------------------------------------------------------------------------
	| --- ClosestPointOnPolygonBoundary: Finds the nearest point on the perimeter of a polygon --- |
	----------------------------------------------------------------------------------------------*/
	BoundaryPoint ClosestPointOnPolygonBoundary(std::span<const Vector2f> vertices, const Vector2f& p)
	{
		BoundaryPoint best;
		const std::size_t count = vertices.size();
		for (std::size_t i = 0; i < count; ++i)
		{
			const Vector2f candidate = ClosestPointOnSegment(p, vertices[i], vertices[NextIndex(i, count)]);
			const float sqrDistance = Vector2f::SqrDistance(p, candidate);
			if (sqrDistance < best.sqrDistance)
			{
				best.point = candidate;
				best.edge = i;
				best.sqrDistance = sqrDistance;
			}
		}
		return best;
	}

	/*----------------------------------------------------------------------------------------------------
	| --- SqrDistanceToPolygonRegion: Computes the squared distance from a point to a polygon region --- |
	----------------------------------------------------------------------------------------------------*/
	float SqrDistanceToPolygonRegion(std::span<const Vector2f> vertices, const Vector2f& p)
	{
		if (IsInsidePolygon(vertices, p))
			return 0.0f;

		return ClosestPointOnPolygonBoundary(vertices, p).sqrDistance;
	}

	/*---------------------------------------------------------------------------------------------------------
	| --- RaycastCircle: Performs a ray-circle intersection test and returns the entry distance parameter --- |
	---------------------------------------------------------------------------------------------------------*/
	bool RaycastCircle(const Vector2f& origin, const Vector2f& direction, const Vector2f& center, float radius, float& t)
	{
		const Vector2f m = origin - center;
		const float b = Vector2f::Dot(m, direction);
		const float c = Vector2f::Dot(m, m) - radius * radius;

		if (c > 0.0f && b > 0.0f)
			return false;

		const float discriminant = b * b - c;
		if (discriminant < 0.0f)
			return false;

		t = std::max(0.0f, -b - std::sqrt(discriminant));
		return true;
	}

	/*----------------------------------------------------------------------------------------------
	| --- RaycastSegment: Performs a ray-segment intersection test for linear shape boundaries --- |
	----------------------------------------------------------------------------------------------*/
	bool RaycastSegment(const Vector2f& origin, const Vector2f& direction, const Vector2f& a, const Vector2f& b, const Vector2f& normal, float& t)
	{
		if (Vector2f::Dot(normal, direction) >= 0.0f)
			return false;

		const Vector2f segment = b - a;
		const float denominator = Vector2f::Cross(direction, segment);
		if (std::fabs(denominator) <= 0.0f)
			return false;

		const Vector2f w = a - origin;
		const float hitT = Vector2f::Cross(w, segment) / denominator;
		const float u = Vector2f::Cross(w, direction) / denominator;

		if (hitT < 0.0f || u < -Math::Epsilon || u > 1.0f + Math::Epsilon)
			return false;

		t = hitT;
		return true;
	}

	/*----------------------------------------------------------------------------------------------------------
	| --- HasSeparatingEdge: Checks for a separating axis along the polygon edges to determine non-overlap --- |
	----------------------------------------------------------------------------------------------------------*/
	bool HasSeparatingEdge(std::span<const Vector2f> first, std::span<const Vector2f> second)
	{
		const std::size_t count = first.size();
		for (std::size_t i = 0; i < count; ++i)
		{
			const Vector2f& a = first[i];
			const Vector2f normal = OutwardNormal(a, first[NextIndex(i, count)]);

			float gap = std::numeric_limits<float>::max();
			for (const Vector2f& vertex : second)
			{
				gap = std::min(gap, Vector2f::Dot(normal, vertex - a));
			}

			if (gap > 0.0f)
				return true;
		}
		return false;
	}

	/*--------------------------------------------------------------------------------------------------------------------
	| --- SqrDistanceBetweenSeparatedPolygons: Calculates the minimum squared distance between two disjoint polygons --- |
	--------------------------------------------------------------------------------------------------------------------*/
	float SqrDistanceBetweenSeparatedPolygons(std::span<const Vector2f> first, std::span<const Vector2f> second)
	{
		float best = std::numeric_limits<float>::max();
		for (const Vector2f& vertex : first)
		{
			best = std::min(best, ClosestPointOnPolygonBoundary(second, vertex).sqrDistance);
		}

		for (const Vector2f& vertex : second)
		{
			best = std::min(best, ClosestPointOnPolygonBoundary(first, vertex).sqrDistance);
		}

		return best;
	}
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------------------------------------
| --- MakeView: Creates a ShapeView2D descriptor for a specific shape index within a shape group --- |
----------------------------------------------------------------------------------------------------*/
CaledonEngine::ShapeView2D CaledonEngine::ShapeQueries2D::MakeView(const PhysicsShapeGroup2D& shapeGroup, int shapeIndex)
{
	const PhysicsShape2D* pShape = shapeGroup.GetShape(shapeIndex);
	if (pShape == nullptr)
		return ShapeView2D{ };

	ShapeView2D view;
	view.type = pShape->shapeType;
	view.vertices = shapeGroup.GetShapeVertices(shapeIndex);
	view.radius = pShape->radius;
	return view;
}

/*-------------------------------------------------------------------------------------------------------------------------
| --- IsConvexCounterClockwise: Validates whether a given vertex collection forms a convex, counter-clockwise polygon --- |
-------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeQueries2D::IsConvexCounterClockwise(std::span<const Vector2f> vertices)
{
	const std::size_t count = vertices.size();
	if (count < 3)
		return false;

	bool hasArea = false;
	for (std::size_t i = 0; i < count; ++i)
	{
		const Vector2f& a = vertices[i];
		const Vector2f edge = vertices[NextIndex(i, count)] - a;

		for (const Vector2f& vertex : vertices)
		{
			const Vector2f toVertex = vertex - a;
			const float cross = Vector2f::Cross(edge, toVertex);
			const float tolerance = 1e-5f * edge.Magnitude() * toVertex.Magnitude();

			if (cross < -tolerance)
				return false;

			if (cross > tolerance)
				hasArea = true;
		}
	}
	return hasArea;
}

/*----------------------------------------------------------------------------------------------------
| --- IsSupported: Checks if a shape view configuration is fully supported by the query pipeline --- |
----------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeQueries2D::IsSupported(const ShapeView2D& shape)
{
	if (!(shape.radius >= 0.0f) || !std::isfinite(shape.radius))
		return false;

	switch (shape.type)
	{
	case PhysicsShapeType2D::Circle:
		return shape.vertices.size() == 1;

	case PhysicsShapeType2D::Polygon:
		return IsConvexCounterClockwise(shape.vertices);

	case PhysicsShapeType2D::Capsule:
	case PhysicsShapeType2D::Edges:
		return shape.vertices.size() == 2 && Vector2f::SqrDistance(shape.vertices[0], shape.vertices[1]) > Math::Epsilon * Math::Epsilon;

	default:
		return false;
	}
}

/*------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Tests whether a world-space point overlaps the interior or boundary of a shape --- |
------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeQueries2D::OverlapPoint(const ShapeView2D& shape, const Vector2f& point)
{
	if (!IsSupported(shape))
		return false;

	const float radiusSqr = shape.radius * shape.radius;

	if (shape.type == PhysicsShapeType2D::Circle)
	{
		return Vector2f::SqrDistance(point, shape.vertices[0]) <= radiusSqr;
	}

	return SqrDistanceToPolygonRegion(shape.vertices, point) <= radiusSqr;
}

/*---------------------------------------------------------------------------------------------------------------------------
| --- ClosestPointOnPerimeter: Finds the point on the outer perimeter of a shape closest to a target reference position --- |
---------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeQueries2D::ClosestPointOnPerimeter(const ShapeView2D& shape, const Vector2f& point, Vector2f& result)
{
	if (!IsSupported(shape))
		return false;

	const float radius = shape.radius;

	if (shape.type == PhysicsShapeType2D::Circle)
	{
		const Vector2f& center = shape.vertices[0];
		const Vector2f offset = point - center;
		const Vector2f direction = (offset.SqrMagnitude() > Math::Epsilon * Math::Epsilon) ? offset.Normalized() : Vector2f::Right();
		result = center + direction * radius;
		return true;
	}

	const std::span<const Vector2f> vertices = shape.vertices;
	const std::size_t count = vertices.size();

	if (!IsInsidePolygon(vertices, point))
	{
		const BoundaryPoint boundary = ClosestPointOnPolygonBoundary(vertices, point);
		if (radius <= 0.0f)
		{
			result = boundary.point;
			return true;
		}

		const Vector2f offset = point - boundary.point;
		const Vector2f direction = (offset.SqrMagnitude() > Math::Epsilon * Math::Epsilon) ? offset.Normalized() : OutwardNormal(vertices[boundary.edge], vertices[NextIndex(boundary.edge, count)]);
		result = boundary.point + direction * radius;
		return true;
	}

	float bestDepth = std::numeric_limits<float>::max();
	Vector2f bestNormal = Vector2f::Right();
	for (std::size_t i = 0; i < count; ++i)
	{
		const Vector2f& a = vertices[i];
		const Vector2f normal = OutwardNormal(a, vertices[NextIndex(i, count)]);
		const float depth = -Vector2f::Dot(normal, point - a);

		if (depth < bestDepth)
		{
			bestDepth = depth;
			bestNormal = normal;
		}
	}

	result = point + bestNormal * (std::max(bestDepth, 0.0f) + radius);
	return true;
}

/*-------------------------------------------------------------------------------------------------
| --- ClosestPointsOfSegments: Finds the closest points between two line segments in 2D space --- |
-------------------------------------------------------------------------------------------------*/
CaledonEngine::SegmentClosestPoints2D CaledonEngine::ShapeQueries2D::ClosestPointsOfSegments(const Vector2f& p1, const Vector2f& q1, const Vector2f& p2, const Vector2f& q2) noexcept
{
	const Vector2f d1 = q1 - p1;
	const Vector2f d2 = q2 - p2;
	const Vector2f r = p1 - p2;
	const float a = Vector2f::Dot(d1, d1);
	const float e = Vector2f::Dot(d2, d2);
	const float f = Vector2f::Dot(d2, r);

	float s = 0.0f;
	float t = 0.0f;
	if (a <= Math::Epsilon && e <= Math::Epsilon)
	{
		// Both segments are points
	}
	else if (a <= Math::Epsilon)
	{
		t = std::clamp(f / e, 0.0f, 1.0f);
	}
	else
	{
		const float c = Vector2f::Dot(d1, r);
		if (e <= Math::Epsilon)
		{
			s = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			const float b = Vector2f::Dot(d1, d2);
			const float denominator = a * e - b * b;
			s = (denominator > 0.0f) ? std::clamp((b * f - c * e) / denominator, 0.0f, 1.0f) : 0.0f;
			t = (b * s + f) / e;
			if (t < 0.0f)
			{
				t = 0.0f;
				s = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = std::clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	return SegmentClosestPoints2D{ s, t, p1 + d1 * s, p2 + d2 * t };
}

/*-----------------------------------------------------------------------------------------------
| --- Raycast: Casts a ray against a shape view and populates hit details upon intersection --- |
-----------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeQueries2D::Raycast(const ShapeView2D& shape, const Vector2f& origin, const Vector2f& direction,
	float maxDistance, bool hitIfStartsInside, ShapeRayHit2D& hit)
{
	if (!IsSupported(shape) || !(maxDistance >= 0.0f))
		return false;

	const Vector2f rayDirection = direction.Normalized();
	if (rayDirection.SqrMagnitude() <= 0.0f)
		return false;

	if (OverlapPoint(shape, origin))
	{
		if (!hitIfStartsInside)
			return false;

		hit.distance = 0.0f;
		hit.point = origin;
		hit.normal = -rayDirection;
		return true;
	}

	constexpr float kNoHit = std::numeric_limits<float>::max();
	float bestT = kNoHit;
	Vector2f bestNormal{ };
	const float radius = shape.radius;

	if (shape.type == PhysicsShapeType2D::Circle)
	{
		const Vector2f& center = shape.vertices[0];
		float t;
		if (RaycastCircle(origin, rayDirection, center, radius, t))
		{
			bestT = t;
			const Vector2f point = origin + rayDirection * t;
			const Vector2f outward = point - center;
			bestNormal = (radius > 0.0f && outward.SqrMagnitude() > 0.0f) ? outward.Normalized() : -rayDirection;
		}
	}
	else
	{
		const std::span<const Vector2f> vertices = shape.vertices;
		const std::size_t count = vertices.size();

		for (std::size_t i = 0; i < count; ++i)
		{
			const Vector2f& a = vertices[i];
			const Vector2f& b = vertices[NextIndex(i, count)];
			const Vector2f normal = OutwardNormal(a, b);

			float t;
			if (RaycastSegment(origin, rayDirection, a + normal * radius, b + normal * radius, normal, t) && t < bestT)
			{
				bestT = t;
				bestNormal = normal;
			}

			if (radius > 0.0f && RaycastCircle(origin, rayDirection, a, radius, t) && t < bestT)
			{
				bestT = t;
				bestNormal = (origin + rayDirection * t - a).Normalized();
			}
		}
	}

	if (bestT == kNoHit || bestT > maxDistance)
		return false;

	hit.distance = bestT;
	hit.point = origin + rayDirection * bestT;
	hit.normal = bestNormal;
	return true;
}

/*------------------------------------------------------------------------------
| --- Overlap: Tests whether two shape views intersect or touch each other --- |
------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeQueries2D::Overlap(const ShapeView2D& shapeA, const ShapeView2D& shapeB)
{
	if (!IsSupported(shapeA) || !IsSupported(shapeB))
		return false;

	const float combinedRadius = shapeA.radius + shapeB.radius;
	const float combinedRadiusSqr = combinedRadius * combinedRadius;
	const bool aIsCircle = shapeA.type == PhysicsShapeType2D::Circle;
	const bool bIsCircle = shapeB.type == PhysicsShapeType2D::Circle;

	if (aIsCircle && bIsCircle)
		return Vector2f::SqrDistance(shapeA.vertices[0], shapeB.vertices[0]) <= combinedRadiusSqr;

	if (aIsCircle)
		return SqrDistanceToPolygonRegion(shapeB.vertices, shapeA.vertices[0]) <= combinedRadiusSqr;

	if (bIsCircle)
		return SqrDistanceToPolygonRegion(shapeA.vertices, shapeB.vertices[0]) <= combinedRadiusSqr;

	if (!HasSeparatingEdge(shapeA.vertices, shapeB.vertices) && !HasSeparatingEdge(shapeB.vertices, shapeA.vertices))
		return true;

	return SqrDistanceBetweenSeparatedPolygons(shapeA.vertices, shapeB.vertices) <= combinedRadiusSqr;
}