/*------------------------------
| File: EdgeCollider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "EdgeCollider2D.h"

#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"

#include <algorithm>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------
| --- Constructor: Constructs this EdgeCollider2D with default values --- |
-------------------------------------------------------------------------*/
CaledonEngine::EdgeCollider2D::EdgeCollider2D()
	: Collider2D{ }
	, m_points{ Vector2f{ -0.5f, 0.0f }, Vector2f{ 0.5f, 0.0f } }
	, m_edgeRadius{ 0.0f }
{ }

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::EdgeCollider2D::GetTypeName() const
{
	static const std::string typeName = "EdgeCollider2D";
	return typeName;
}

/*--------------------------------------------------------------------
| --- SetEdgeRadius: Sets the edge radius of this EdgeCollider2D --- |
--------------------------------------------------------------------*/
void CaledonEngine::EdgeCollider2D::SetEdgeRadius(float edgeRadius)
{
	m_edgeRadius = std::max(edgeRadius, 0.0f);
	RefreshBounds();
}

/*-------------------------------------------------------------------------
| --- SetPoints: Sets the points of the chain for this EdgeCollider2D --- |
-------------------------------------------------------------------------*/
void CaledonEngine::EdgeCollider2D::SetPoints(const std::vector<Vector2f>& points)
{
	m_points = points;
	RefreshBounds();
}

/*-----------------------------------------------------------------------------------------
| --- Reset: Resets the points of the chain for this EdgeCollider2D to an empty state --- |
-----------------------------------------------------------------------------------------*/
void CaledonEngine::EdgeCollider2D::Reset()
{
	SetPoints({ Vector2f{ -0.5f, 0.0f }, Vector2f{ 0.5f, 0.0f } });
}



/*--------------------------------------
| --- Protected Method Definitions --- |
--------------------------------------*/
/*-------------------------------------------------------------------------------------------------
| --- AddLocalShapes: Adds the local shapes of this EdgeCollider2D to the physics shape group --- |
-------------------------------------------------------------------------------------------------*/
int CaledonEngine::EdgeCollider2D::AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const
{
	int added = 0;
	for (std::size_t i = 0; i + 1 < m_points.size(); ++i)
	{
		const Vector2f segment[2] = { GetOffset() + m_points[i], GetOffset() + m_points[i + 1] };
		if (segment[0] == segment[1])
			continue;

		if (physicsShapeGroup.AddEdges(segment, m_edgeRadius) >= 0)
			++added;
	}
	return added;
}