/*------------------------------
| File: CapsuleCollider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "CapsuleCollider2D.h"

#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"

#include <algorithm>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------------
| --- Constructor: Constructs this CapsuleCollider2D with default values --- |
----------------------------------------------------------------------------*/
CaledonEngine::CapsuleCollider2D::CapsuleCollider2D()
	: Collider2D{ }
	, m_direction{ CapsuleDirection2D::Vertical }
	, m_size{ 1.0f, 2.0f }
{ }

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::CapsuleCollider2D::GetTypeName() const
{
	static const std::string typeName = "CapsuleCollider2D";
	return typeName;
}

/*--------------------------------------------------------------------
| --- SetDirection: Sets the direction of this CapsuleCollider2D --- |
--------------------------------------------------------------------*/
void CaledonEngine::CapsuleCollider2D::SetDirection(CapsuleDirection2D direction)
{
	m_direction = direction;
	RefreshBounds();
}

/*----------------------------------------------------------
| --- SetSize: Sets the size of this CapsuleCollider2D --- |
----------------------------------------------------------*/
void CaledonEngine::CapsuleCollider2D::SetSize(const Vector2f& size)
{
	m_size = Vector2f{ std::max(size.x, 0.0f), std::max(size.y, 0.0f) };
	RefreshBounds();
}



/*--------------------------------------
| --- Protected Method Definitions --- |
--------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------
| --- AddLocalShapes:
----------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::CapsuleCollider2D::AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const
{
	const bool vertical = (m_direction == CapsuleDirection2D::Vertical);
	const float radius = 0.5f * (vertical ? m_size.x : m_size.y);
	const float halfLength = 0.5f * ((vertical ? m_size.y : m_size.x) - 2.0f * radius);
	if (!(radius > 0.0f))
		return 0;

	if (!(halfLength > 0.0f))
		return (physicsShapeGroup.AddCircle(GetOffset(), radius) >= 0) ? 1 : 0;

	const Vector2f axis = vertical ? Vector2f{ 0.0f, halfLength } : Vector2f{ halfLength, 0.0f };
	return (physicsShapeGroup.AddCapsule(GetOffset() - axis, GetOffset() + axis, radius) >= 0) ? 1 : 0;
}