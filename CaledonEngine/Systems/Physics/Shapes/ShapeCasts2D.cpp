/*------------------------------
| File: ShapeCasts2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ShapeCasts2D.h"

#include "Systems/Physics/Shapes/ShapeDistance2D.h"
#include "Utilities/Math/Constants.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace
{
	using namespace CaledonEngine;

	/*-----------------------------------------------------------------------------------------------------------------------------------
	| --- ConvexHull: Returns the convex hull of a set of points in counterclockwise order, using Andrew's monotone chain algorithm --- |
	-----------------------------------------------------------------------------------------------------------------------------------*/
	std::vector<Vector2f> ConvexHull(std::vector<Vector2f> points)
	{
		std::sort(points.begin(), points.end(), [](const Vector2f& a, const Vector2f& b)
			{
				return (a.x != b.x) ? a.x < b.x : a.y < b.y;
			});

		float extent = 0.0f;
		for (const Vector2f& point : points)
		{
			extent = std::max(extent, std::max(std::fabs(point.x), std::fabs(point.y)));
		}
		const float tolerance = Math::TightTolerance * std::max(extent, 1.0f);

		std::vector<Vector2f> unique;
		for (const Vector2f& point : points)
		{
			if (unique.empty() || Vector2f::SqrDistance(unique.back(), point) > tolerance * tolerance)
			{
				unique.push_back(point);
			}
		}

		if (unique.size() <= 2)
			return unique;

		const auto isNotLeftTurn = [](const Vector2f& a, const Vector2f& b, const Vector2f& c)
			{
				const Vector2f ab = b - a;
				const Vector2f ac = c - a;
				return Vector2f::Cross(ab, ac) <= 2.0f * Math::LooseTolerance * ab.Magnitude() * ac.Magnitude();
			};

		std::vector<Vector2f> hull(2 * unique.size());
		std::size_t k = 0;

		for (std::size_t i = 0; i < unique.size(); ++i)
		{
			while (k >= 2 && isNotLeftTurn(hull[k - 2], hull[k - 1], unique[i]))
			{
				--k;
			}
			hull[k++] = unique[i];
		}

		for (std::size_t i = unique.size() - 1, lower = k + 1; i > 0; --i)
		{
			while (k >= lower && isNotLeftTurn(hull[k - 2], hull[k - 1], unique[i - 1]))
			{
				--k;
			}
			hull[k++] = unique[i - 1];
		}

		hull.resize(k - 1);
		return hull;
	}
}

/*-----------------------------------------------------------------------------------------------------------------------------
| --- Cast: Casts the moving shape along the given direction and distance and returns true if it touches the target shape --- |
-----------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeCasts2D::Cast(const ShapeView2D& moving, const Vector2f& direction, float maxDistance, const ShapeView2D& target, bool hitIfStartsInside, ShapeCastHit2D& hit)
{
	hit = ShapeCastHit2D{ };
	if (!ShapeQueries2D::IsSupported(moving) || !ShapeQueries2D::IsSupported(target) || !(maxDistance >= 0.0f))
		return false;

	const Vector2f unitDirection = direction.Normalized();
	if (!(unitDirection.SqrMagnitude() > 0.0f))
		return false;

	const std::span<const Vector2f> movingCore = moving.vertices;
	const std::span<const Vector2f> targetCore = target.vertices;

	std::vector<Vector2f> differences;
	differences.reserve(movingCore.size() * targetCore.size());

	for (const Vector2f& b : targetCore)
	{
		for (const Vector2f& a : movingCore)
		{
			differences.push_back(b - a);
		}
	}

	const std::vector<Vector2f> hull = ConvexHull(std::move(differences));
	const float radius = moving.radius + target.radius;

	ShapeView2D difference;
	difference.vertices = hull;
	difference.radius = radius;

	if (hull.size() == 1)
	{
		difference.type = PhysicsShapeType2D::Circle;
	}
	else if (hull.size() == 2)
	{
		difference.type = (radius > 0.0f) ? PhysicsShapeType2D::Capsule : PhysicsShapeType2D::Edges;
	}
	else
	{
		difference.type = PhysicsShapeType2D::Polygon;
	}

	if (hull.empty() || !ShapeQueries2D::IsSupported(difference))
		return false;

	ShapeRayHit2D rayHit;
	if (!ShapeQueries2D::Raycast(difference, Vector2f::Zero(), unitDirection, maxDistance, hitIfStartsInside, rayHit))
		return false;

	std::vector<Vector2f> slidVertices(moving.vertices.begin(), moving.vertices.end());
	for (Vector2f& vertex : slidVertices)
	{
		vertex = vertex + unitDirection * rayHit.distance;
	}

	ShapeView2D slid = moving;
	slid.vertices = slidVertices;

	ColliderDistance2D separation;
	hit.distance = rayHit.distance;
	hit.normal = rayHit.normal;
	if (ShapeDistance2D::Compute(slid, target, separation))
	{
		hit.point = separation.pointB;
	}
	else
	{
		return false;
	}
	return true;
}