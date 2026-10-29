/*------------------------------
| File: BoxCollider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "BoxCollider2D.h"

#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"

#include <algorithm>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------
| --- Constructor: Constructs this BoxCollider2D with default values --- |
------------------------------------------------------------------------*/
CaledonEngine::BoxCollider2D::BoxCollider2D()
	: Collider2D{ }
	, m_autoTiling{ false }
	, m_edgeRadius{ 0.0f }
	, m_size{ 1.0f, 1.0f }
{ }

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::BoxCollider2D::GetTypeName() const
{
    static const std::string typeName = "BoxCollider2D";
    return typeName;
}

/*-------------------------------------------------------------------
| --- SetEdgeRadius: Sets the edge radius of this BoxCollider2D --- |
-------------------------------------------------------------------*/
void CaledonEngine::BoxCollider2D::SetEdgeRadius(float edgeRadius)
{
	m_edgeRadius = std::max(edgeRadius, 0.0f);
	RefreshBounds();
}

/*------------------------------------------------------
| --- SetSize: Sets the size of this BoxCollider2D --- |
------------------------------------------------------*/
void CaledonEngine::BoxCollider2D::SetSize(const Vector2f& size)
{
	m_size = Vector2f{ std::max(size.x, 0.0f), std::max(size.y, 0.0f) };
	RefreshBounds();
}

/*-----------------------------------------------------------------------
| --- AddLocalShapes: 
-----------------------------------------------------------------------*/
int CaledonEngine::BoxCollider2D::AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const
{
	return (physicsShapeGroup.AddBox(GetOffset(), m_size, 0.0f, m_edgeRadius) >= 0) ? 1 : 0;
}