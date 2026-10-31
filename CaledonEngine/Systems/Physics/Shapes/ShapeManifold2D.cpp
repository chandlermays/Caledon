/*------------------------------
| File: ShapeManifold2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ShapeManifold2D.h"

#include <cfloat>
#include <cmath>

namespace
{
	using namespace CaledonEngine;

	constexpr float kReferenceTolerance = 0.0005f;

	/*------------------------------------------------------------------------------------------------
	| --- Dot: 
	------------------------------------------------------------------------------------------------*/
	float Dot(const Vector2f& a, const Vector2f& b) noexcept
	{
		return a.x * b.x + a.y * b.y;
	}

	/*------------------------------------------------------------------------------------------------
	| --- EdgeNormal: The unit normal that points out of a counter-clockwise polygon edge a -> b --- |
	------------------------------------------------------------------------------------------------*/
	Vector2f EdgeNormal(const Vector2f& a, const Vector2f& b) noexcept
	{
		const Vector2f edge = b - a;
		const float length = std::sqrt(edge.x * edge.x + edge.y * edge.y);
		if (!(length > 0.0f))
			return Vector2f{ 0.0f, 0.0f };

		return Vector2f{ edge.y / length, -edge.x / length };
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
		const float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		if (distance - totalRadius > 0.0f)
			return false;

		// Perfectly stacked centers have no direction, so any direction is as good as another
		const Vector2f normal = (distance > FLT_EPSILON) ? delta * (1.0f / distance) : Vector2f{ 1.0f, 0.0f };

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
		float faceSeparation = -FLT_MAX;
		for (std::size_t i = 0; i < count; ++i)
		{
			const float separation = Dot(EdgeNormal(vertices[i], vertices[(i + 1) % count]), center - vertices[i]);
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

		if (faceSeparation > FLT_EPSILON)
		{
			const float beyondV1 = Dot(center - v1, v2 - v1);
			const float beyondV2 = Dot(center - v2, v1 - v2);
			if (beyondV1 <= 0.0f || beyondV2 <= 0.0f)
			{
				const Vector2f& corner = (beyondV1 <= 0.0f) ? v1 : v2;
				const Vector2f delta = center - corner;
				const float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
				if (distance > totalRadius)
					return false;

				normal = (distance > FLT_EPSILON) ? delta * (1.0f / distance) : faceNormal;
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
		float separation = -FLT_MAX;
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

			float nearest = FLT_MAX;
			for (const Vector2f& vertex : b)
				nearest = std::fmin(nearest, Dot(normal, vertex - a[i]));

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
		const float distance0 = Dot(normal, in[0]) - offset;
		const float distance1 = Dot(normal, in[1]) - offset;

		int count = 0;
		if (distance0 <= 0.0f)
			out[count++] = in[0];
		if (distance1 <= 0.0f)
			out[count++] = in[1];

		if (distance0 * distance1 < 0.0f)
		{
			const float t = distance0 / (distance0 - distance1);
			out[count++] = in[0] + (in[1] - in[0]) * t;
		}
		return count;
	}

	struct SegmentPair
	{
		float s = 0.0f;
		float t = 0.0f;
		Vector2f point1{ };
		Vector2f point2{ };
	};

	/*-------------------------------------------------------------------------------------------------------------------------
	| --- ClosestPointsOfSegments:
	-------------------------------------------------------------------------------------------------------------------------*/
	SegmentPair ClosestPointsOfSegments(const Vector2f& p1, const Vector2f& q1, const Vector2f& p2, const Vector2f& q2) noexcept
	{
		const Vector2f d1 = q1 - p1;
		const Vector2f d2 = q2 - p2;
		const Vector2f r = p1 - p2;
		const float a = Dot(d1, d1);
		const float e = Dot(d2, d2);
		const float f = Dot(d2, r);

		float s = 0.0f;
		float t = 0.0f;
		if (a <= FLT_EPSILON && e <= FLT_EPSILON)
		{
			// Both segments are single points
		}
		else if (a <= FLT_EPSILON)
		{
			t = std::fmin(std::fmax(f / e, 0.0f), 1.0f);
		}
		else
		{
			const float c = Dot(d1, r);
			if (e <= FLT_EPSILON)
			{
				s = std::fmin(std::fmax(-c / a, 0.0f), 1.0f);
			}
			else
			{
				const float b = Dot(d1, d2);
				const float denominator = a * e - b * b;
				s = (denominator > 0.0f) ? std::fmin(std::fmax((b * f - c * e) / denominator, 0.0f), 1.0f) : 0.0f;
				t = (b * s + f) / e;
				if (t < 0.0f)
				{
					t = 0.0f;
					s = std::fmin(std::fmax(-c / a, 0.0f), 1.0f);
				}
				else if (t > 1.0f)
				{
					t = 1.0f;
					s = std::fmin(std::fmax((b - c) / a, 0.0f), 1.0f);
				}
			}
		}

		SegmentPair result;
		result.s = s;
		result.t = t;
		result.point1 = p1 + d1 * s;
		result.point2 = p2 + d2 * t;
		return result;
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
		float mostOpposite = FLT_MAX;
		for (std::size_t i = 0; i < incVertices.size(); ++i)
		{
			const float alignment = Dot(referenceNormal, EdgeNormal(incVertices[i], incVertices[(i + 1) % incVertices.size()]));
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
			const SegmentPair closest = ClosestPointsOfSegments(r1, r2, incidentSegment[0], incidentSegment[1]);
			const bool referenceEnd = (closest.s == 0.0f || closest.s == 1.0f);
			const bool incidentEnd = (closest.t == 0.0f || closest.t == 1.0f);
			if (referenceEnd && incidentEnd)
			{
				const Vector2f delta = closest.point2 - closest.point1;
				const float distance = std::sqrt(Dot(delta, delta));
				if (distance > totalRadius)
					return false;

				const Vector2f toIncident = (distance > FLT_EPSILON) ? delta * (1.0f / distance) : referenceNormal;
				manifold.normal = referenceIsB ? toIncident * -1.0f : toIncident;
				manifold.pointCount = 1;
				manifold.points[0].point = Midpoint(closest.point1, reference.radius, closest.point2, incident.radius, toIncident);
				manifold.points[0].separation = distance - totalRadius;
				return true;
			}
		}

		const Vector2f tangent = (r2 - r1) * (1.0f / std::sqrt(Dot(r2 - r1, r2 - r1)));
		Vector2f clippedA[2];
		Vector2f clippedB[2];

		if (ClipSegment(incidentSegment, clippedA, tangent * -1.0f, -Dot(tangent, r1)) < 2)
			return false;

		if (ClipSegment(clippedA, clippedB, tangent, Dot(tangent, r2)) < 2)
			return false;

		manifold.normal = referenceIsB ? referenceNormal * -1.0f : referenceNormal;
		manifold.pointCount = 0;
		for (const Vector2f& clipped : clippedB)
		{
			const float coreSeparation = Dot(referenceNormal, clipped - r1);
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
		manifold.normal = manifold.normal * -1.0f;
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
