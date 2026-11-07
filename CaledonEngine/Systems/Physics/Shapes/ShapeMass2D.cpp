/*------------------------------
| File: ShapeMass2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ShapeMass2D.h"

#include "Utilities/Math/Constants.h"

#include <cmath>

namespace
{
	using namespace CaledonEngine;

	/*-----------------------------------------------------------------
	| --- ComputeCircle: Computes the mass data of a circle shape --- |
	-----------------------------------------------------------------*/
	bool ComputeCircle(const ShapeView2D& shape, ShapeMassData2D& massData)
	{
		if (shape.vertices.empty() || !(shape.radius > 0.0f))
			return false;

		const Vector2f& center = shape.vertices[0];
		const float radiusSqr = shape.radius * shape.radius;

		massData.area = Math::Pi * radiusSqr;
		massData.centroid = center;
		massData.inertia = massData.area * (0.5f * radiusSqr + center.SqrMagnitude());
		return true;
	}

	/*-------------------------------------------------------------------
	| --- ComputeCapsule: Computes the mass data of a capsule shape --- |
	-------------------------------------------------------------------*/
	bool ComputeCapsule(const ShapeView2D& shape, ShapeMassData2D& massData)
	{
		if (shape.vertices.size() != 2 || !(shape.radius > 0.0f))
			return false;

		const Vector2f& p1 = shape.vertices[0];
		const Vector2f& p2 = shape.vertices[1];
		const float radius = shape.radius;
		const float rr = radius * radius;
		const float length = (p2 - p1).Magnitude();
		const float h = 0.5f * length;

		const float discArea = Math::Pi * rr;
		const float boxArea = 2.0f * radius * length;
		const float halfDiscCentroid = 4.0f * radius / (3.0f * Math::Pi);
		const float discInertia = discArea * (0.5f * rr + h * h + 2.0f * h * halfDiscCentroid);
		const float boxInertia = boxArea * (4.0f * rr + length * length) / 12.0f;

		const Vector2f center = (p1 + p2) * 0.5f;
		massData.area = discArea + boxArea;
		massData.centroid = center;
		massData.inertia = discInertia + boxInertia + massData.area * center.SqrMagnitude();
		return true;
	}

	/*-------------------------------------------------------------------
	| --- ComputePolygon: Computes the mass data of a polygon shape --- |
	-------------------------------------------------------------------*/
	bool ComputePolygon(const ShapeView2D& shape, ShapeMassData2D& massData)
	{
		const std::size_t count = shape.vertices.size();
		if (count < 3)
			return false;

		const Vector2f reference = shape.vertices[0];

		float area = 0.0f;
		float centerX = 0.0f;
		float centerY = 0.0f;
		float inertia = 0.0f;

		for (std::size_t i = 1; i + 1 < count; ++i)
		{
			const Vector2f e1 = shape.vertices[i] - reference;
			const Vector2f e2 = shape.vertices[i + 1] - reference;

			const float twiceArea = e1.x * e2.y - e1.y * e2.x;
			const float triangleArea = 0.5f * twiceArea;

			area += triangleArea;
			centerX += triangleArea * (e1.x + e2.x) / 3.0f;
			centerY += triangleArea * (e1.y + e2.y) / 3.0f;

			const float xx = e1.x * e1.x + e1.x * e2.x + e2.x * e2.x;
			const float yy = e1.y * e1.y + e1.y * e2.y + e2.y * e2.y;
			inertia += (twiceArea / 12.0f) * (xx + yy);
		}

		if (area < 0.0f)
		{
			area = -area;
			centerX = -centerX;
			centerY = -centerY;
			inertia = -inertia;
		}

		if (!(area > 0.0f))
			return false;

		const Vector2f centroidFromReference{ centerX / area, centerY / area };

		massData.area = area;
		massData.centroid = reference + centroidFromReference;

		massData.inertia = inertia + area * (massData.centroid.SqrMagnitude() - centroidFromReference.SqrMagnitude());

		return true;
	}
}

/*------------------------------------------------------------
| --- Compute: Computes the mass data of the given shape --- |
------------------------------------------------------------*/
bool ShapeMass2D::Compute(const ShapeView2D& shape, ShapeMassData2D& massData)
{
	switch (shape.type)
	{
	case PhysicsShapeType2D::Circle:
		return ComputeCircle(shape, massData);

	case PhysicsShapeType2D::Capsule:
		return ComputeCapsule(shape, massData);

	case PhysicsShapeType2D::Polygon:
		return ComputePolygon(shape, massData);

	default:
		return false;
	}
}