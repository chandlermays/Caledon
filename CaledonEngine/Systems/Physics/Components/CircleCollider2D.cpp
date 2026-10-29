/*------------------------------
| File: CircleCollider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "CircleCollider2D.h"

#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------------
| --- Constructor: Constructs this CircleCollider2D with default values --- |
---------------------------------------------------------------------------*/
CaledonEngine::CircleCollider2D::CircleCollider2D()
    : Collider2D{ }
    , m_radius{ 0.5f }
{ }

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::CircleCollider2D::GetTypeName() const
{
    static const std::string typeName = "CircleCollider2D";
    return typeName;
}

/*-------------------------------------------------------------
| --- SetRadius: Sets the radius of this CircleCollider2D --- |
-------------------------------------------------------------*/
void CaledonEngine::CircleCollider2D::SetRadius(float radius)
{
    m_radius = std::max(radius, 0.0f);
    RefreshBounds();
}

/*---------------------------------------------------------------------------
| --- AddLocalShapes: 
---------------------------------------------------------------------------*/
int CaledonEngine::CircleCollider2D::AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const
{
    return (physicsShapeGroup.AddCircle(GetOffset(), m_radius) >= 0) ? 1 : 0;
}