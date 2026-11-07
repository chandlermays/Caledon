/*------------------------------
| File: ShapeManifold2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ShapeManifold2D.h"

#include "Utilities/Math/Constants.h"

#include <cmath>

namespace
{
	using namespace CaledonEngine;

	constexpr float kReferenceTolerance = 0.0005f;

	/*------------------------------------------------------------------------------------------------
	| --- EdgeNormal: The unit normal that points out of a counter-clockwise polygon edge a -> b --- |
	------------------------------------------------------------------------------------------------*/
	Vector2f EdgeNormal(const Vector2f& a, const Vector2f& b) noexcept
	{
		const Vector2f edge = b - a;
		return Vector2f{ edge.y, -edge.x }.Normalized();			// A zero-length edge gives the zero vector
	}

	/*-------------------------------------------------------------------------------------------------------
	| --- Midpoint: The point halfway between the two surfaces, given the two core points and the radii --- |
	-------------------------------------------------------------------------------------------------------*/
	Vector2f Midpoint(const Vector2f& coreA, float radiusA, const Vector2f& coreB, float radiusB, const Vector2f& normal) noexcept
	{
		const Vector2f surfaceA = coreA + normal * radiusA;
		const Vector2f surfaceB = coreB - normal * radiusB;
		return (surfaceA + surfaceB) * 0.5f;
	}

	/*---------------------------------------------------------------------------------------------------
	| --- CollideCircles: Two circles touch when their centers are closer than the sum of the radii --- |
	---------------------------------------------------------------------------------------------------*/
	bool CollideCircles(const ShapeView2D& a, const ShapeView2D& b, ShapeManifold2D& manifold)
	{
		const Vector2f centerA = a.vertices[0];
		const Vector2f centerB = b.vertices[0];
		const float totalRadius = a.radius + b.radius;

		const Vector2f delta = centerB - centerA;
		const float distance = delta.Magnitude();
		if (distance > totalRadius)
			return false;

		// Perfectly stacked centers have no direction, so any direction is as good as another
		const Vector2f normal = (distance > Math::Epsilon) ? delta / distance : Vector2f::Right();

		manifold.normal = normal;
		manifold.pointCount = 1;
		manifold.points[0].point = Midpoint(centerA, a.radius, centerB, b.radius, normal);
		manifold.points[0].separation = distance - totalRadius;
		return true;
	}

	/*-------------------------------------------------------------------------------------------------------------------------------
	| --- CollidePolygonCircle: Finds which part of the polygon the circle's center is closest to: a face or one of its corners --- |
	-------------------------------------------------------------------------------------------------------------------------------*/
	bool CollidePolygonCircle(const ShapeView2D& polygon, const ShapeView2D& circle, ShapeManifold2D& manifold)
	{
		const std::span<const Vector2f> vertices = polygon.vertices;
		const std::size_t count = vertices.size();
		const Vector2f center = circle.vertices[0];
		const float totalRadius = polygon.radius + circle.radius;

		std::size_t faceIndex = 0;
		float faceSeparation = -Math::MaxFloat;
		for (std::size_t i = 0; i < count; ++i)
		{
			const float separation = Vector2f::Dot(EdgeNormal(vertices[i], vertices[(i + 1) % count]), center - vertices[i]);
			if (separation > totalRadius)
				return false;

			if (separation > faceSeparation)
			{
				faceSeparation = separation;
				faceIndex = i;
			}
		}

		const Vector2f& v1 = vertices[faceIndex];
		const Vector2f& v2 = vertices[(faceIndex + 1) % count];
		const Vector2f faceNormal = EdgeNormal(v1, v2);

		Vector2f normal = faceNormal;
		Vector2f corePoint = center - faceNormal * faceSeparation;
		float coreSeparation = faceSeparation;

		if (faceSeparation > Math::Epsilon)
		{
			const float beyondV1 = Vector2f::Dot(center - v1, v2 - v1);
			const float beyondV2 = Vector2f::Dot(center - v2, v1 - v2);

			if (beyondV1 <= 0.0f || beyondV2 <= 0.0f)
			{
				const Vector2f& corner = (beyondV1 <= 0.0f) ? v1 : v2;
				const Vector2f delta = center - corner;

				const float distance = delta.Magnitude();
				if (distance > totalRadius)
					return false;

				normal = (distance > Math::Epsilon) ? delta / distance : faceNormal;
				corePoint = corner;
				coreSeparation = distance;
			}
		}

		manifold.normal = normal;
		manifold.pointCount = 1;
		manifold.points[0].point = Midpoint(corePoint, polygon.radius, center, circle.radius, normal);
		manifold.points[0].separation = coreSeparation - totalRadius;
		return true;
	}

	struct EdgeSeparation
	{
		std::size_t edge = 0;
		float separation = -Math::MaxFloat;
	};

	/*------------------------------------------------------------------------------------------------------------------------
	| --- FindMaxSeparation:
	------------------------------------------------------------------------------------------------------------------------*/
	EdgeSeparation FindMaxSeparation(std::span<const Vector2f> a, std::span<const Vector2f> b)
	{
		EdgeSeparation best;
		for (std::size_t i = 0; i < a.size(); ++i)
		{
			const Vector2f normal = EdgeNormal(a[i], a[(i + 1) % a.size()]);

			float nearest = Math::MaxFloat;

			for (const Vector2f& vertex : b)
			{
				nearest = std::fmin(nearest, Vector2f::Dot(normal, vertex - a[i]));
			}

			if (nearest > best.separation)
			{
				best.separation = nearest;
				best.edge = i;
			}
		}
		return best;
	}

	/*-------------------------------------------------------------------------------------------------------------------------
	| --- ClipSegment:
	-------------------------------------------------------------------------------------------------------------------------*/
	int ClipSegment(const Vector2f in[2], Vector2f out[2], const Vector2f& normal, float offset) noexcept
	{
		const float distance0 = Vector2f::Dot(normal, in[0]) - offset;
		const float distance1 = Vector2f::Dot(normal, in[1]) - offset;

		int count = 0;
		if (distance0 <= 0.0f)
		{
			out[count++] = in[0];
		}

		if (distance1 <= 0.0f)
		{
			out[count++] = in[1];
		}

		if (distance0 * distance1 < 0.0f)
		{
			const float t = distance0 / (distance0 - distance1);
			out[count++] = in[0] + (in[1] - in[0]) * t;
		}
		return count;
	}

	/*-------------------------------------------------------------------------------------------------------------------------------
	| --- CollidePolygons:
	-------------------------------------------------------------------------------------------------------------------------------*/
	bool CollidePolygons(const ShapeView2D& a, const ShapeView2D& b, ShapeManifold2D& manifold)
	{
		const float totalRadius = a.radius + b.radius;

		const EdgeSeparation separationA = FindMaxSeparation(a.vertices, b.vertices);
		if (separationA.separation > totalRadius)
			return false;

		const EdgeSeparation separationB = FindMaxSeparation(b.vertices, a.vertices);
		if (separationB.separation > totalRadius)
			return false;

		const bool referenceIsB = separationB.separation > separationA.separation + kReferenceTolerance;
		const ShapeView2D& reference = referenceIsB ? b : a;
		const ShapeView2D& incident = referenceIsB ? a : b;
		const std::size_t referenceEdge = referenceIsB ? separationB.edge : separationA.edge;

		const std::span<const Vector2f> refVertices = reference.vertices;
		const std::span<const Vector2f> incVertices = incident.vertices;

		const Vector2f& r1 = refVertices[referenceEdge];
		const Vector2f& r2 = refVertices[(referenceEdge + 1) % refVertices.size()];
		const Vector2f referenceNormal = EdgeNormal(r1, r2);

		std::size_t incidentEdge = 0;
		float mostOpposite = Math::MaxFloat;
		for (std::size_t i = 0; i < incVertices.size(); ++i)
		{
			const float alignment = Vector2f::Dot(referenceNormal, EdgeNormal(incVertices[i], incVertices[(i + 1) % incVertices.size()]));
			if (alignment < mostOpposite)
			{
				mostOpposite = alignment;
				incidentEdge = i;
			}
		}

		const Vector2f incidentSegment[2] = { incVertices[incidentEdge], incVertices[(incidentEdge + 1) % incVertices.size()] };

		const float referenceSeparation = referenceIsB ? separationB.separation : separationA.separation;
		if (referenceSeparation > kReferenceTolerance)
		{
			const SegmentClosestPoints2D closest = ShapeQueries2D::ClosestPointsOfSegments(r1, r2, incidentSegment[0], incidentSegment[1]);
			const bool referenceEnd = (closest.s == 0.0f || closest.s == 1.0f);
			const bool incidentEnd = (closest.t == 0.0f || closest.t == 1.0f);

			if (referenceEnd && incidentEnd)
			{
				const Vector2f delta = closest.pointOnSecond - closest.pointOnFirst;
				const float distance = delta.Magnitude();
				if (distance > totalRadius)
					return false;

				const Vector2f toIncident = (distance > Math::Epsilon) ? delta / distance : referenceNormal;
				manifold.normal = referenceIsB ? -toIncident : toIncident;
				manifold.pointCount = 1;
				manifold.points[0].point = Midpoint(closest.pointOnFirst, reference.radius, closest.pointOnSecond, incident.radius, toIncident);
				manifold.points[0].separation = distance - totalRadius;
				return true;
			}
		}

		const Vector2f tangent = (r2 - r1).Normalized();
		Vector2f clippedA[2];
		Vector2f clippedB[2];

		if (ClipSegment(incidentSegment, clippedA, -tangent, -Vector2f::Dot(tangent, r1)) < 2)
			return false;

		if (ClipSegment(clippedA, clippedB, tangent, Vector2f::Dot(tangent, r2)) < 2)
			return false;

		manifold.normal = referenceIsB ? -referenceNormal : referenceNormal;
		manifold.pointCount = 0;
		for (const Vector2f& clipped : clippedB)
		{
			const float coreSeparation = Vector2f::Dot(referenceNormal, clipped - r1);
			const float separation = coreSeparation - totalRadius;
			if (separation > 0.0f)
				continue;

			const Vector2f referenceCore = clipped - referenceNormal * coreSeparation;
			ShapeManifoldPoint2D& point = manifold.points[manifold.pointCount++];
			point.separation = separation;
			point.point = Midpoint(referenceCore, reference.radius, clipped, incident.radius, referenceNormal);
		}

		return manifold.pointCount > 0;
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------
| --- Collide:
---------------------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeManifold2DQueries::Collide(const ShapeView2D& shapeA, const ShapeView2D& shapeB, ShapeManifold2D& manifold)
{
	manifold.Clear();

	if (!ShapeQueries2D::IsSupported(shapeA) || !ShapeQueries2D::IsSupported(shapeB))
		return false;

	const bool circleA = shapeA.type == PhysicsShapeType2D::Circle;
	const bool circleB = shapeB.type == PhysicsShapeType2D::Circle;

	bool touching = false;
	if (circleA && circleB)
	{
		touching = CollideCircles(shapeA, shapeB, manifold);
	}
	else if (circleB)
	{
		touching = CollidePolygonCircle(shapeA, shapeB, manifold);
	}
	else if (circleA)
	{
		touching = CollidePolygonCircle(shapeB, shapeA, manifold);
		manifold.normal = -manifold.normal;
	}
	else
	{
		touching = CollidePolygons(shapeA, shapeB, manifold);
	}

	if (!touching)
	{
		manifold.Clear();
	}

	return touching;
}