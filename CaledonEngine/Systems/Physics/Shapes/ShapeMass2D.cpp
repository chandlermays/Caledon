/*------------------------------
| File: ShapeMass2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ShapeMass2D.h"

#include "Utilities/Math/Constants.h"

#include <cmath>

namespace
{
	/*-----------------------------------------------------------------------------------------------------------------------
	| --- ComputeCircle:
	-----------------------------------------------------------------------------------------------------------------------*/
	bool ComputeCircle(const CaledonEngine::ShapeView2D& shape, CaledonEngine::ShapeMassData2D& massData)
	{
		if (shape.vertices.empty() || !(shape.radius > 0.0f))
			return false;

		const CaledonEngine::Vector2f& center = shape.vertices[0];
		const float radiusSqr = shape.radius * shape.radius;

		massData.area = CaledonEngine::Math::Pi * radiusSqr;
		massData.centroid = center;
		massData.inertia = massData.area * (0.5f * radiusSqr + center.x * center.x + center.y * center.y);
		return true;
	}

	/*-----------------------------------------------------------------------------------------------------------------------
	| --- ComputePolygon: 
	-----------------------------------------------------------------------------------------------------------------------*/
	bool ComputePolygon(const CaledonEngine::ShapeView2D& shape, CaledonEngine::ShapeMassData2D& massData)
	{
		const std::size_t count = shape.vertices.size();
		if (count < 3)
			return false;

		const CaledonEngine::Vector2f reference = shape.vertices[0];

		float area = 0.0f;
		float centerX = 0.0f;
		float centerY = 0.0f;
		float inertia = 0.0f;

		for (std::size_t i = 1; i + 1 < count; ++i)
		{
			const CaledonEngine::Vector2f e1 = shape.vertices[i] - reference;
			const CaledonEngine::Vector2f e2 = shape.vertices[i + 1] - reference;

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

		const CaledonEngine::Vector2f centroidFromReference{ centerX / area, centerY / area };

		massData.area = area;
		massData.centroid = reference + centroidFromReference;
		massData.inertia = inertia + area * (massData.centroid.x * massData.centroid.x + massData.centroid.y * massData.centroid.y
				- centroidFromReference.x * centroidFromReference.x - centroidFromReference.y * centroidFromReference.y);

		return true;
	}
}

/*---------------------------------------------------------------------------------------------------------------
| --- Compute: Computes the mass data of a shape; returns false if the shape has no area or isn't supported --- |
---------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ShapeMass2D::Compute(const ShapeView2D& shape, ShapeMassData2D& massData)
{
	switch (shape.type)
	{
	case PhysicsShapeType2D::Circle:
		return ComputeCircle(shape, massData);

	case PhysicsShapeType2D::Polygon:
		return ComputePolygon(shape, massData);

	default:
		return false;
	}
}
