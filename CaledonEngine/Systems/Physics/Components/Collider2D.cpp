/*------------------------------
| File: Collider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "Collider2D.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------
| --- Constructor: Constructs this Collider2D with default values --- |
---------------------------------------------------------------------*/
CaledonEngine::Collider2D::Collider2D()
	: m_pAttachedRigidBody{ nullptr }
	, m_bounceCombine{ PhysicsMaterialCombine2D::Maximum }
	, m_bounciness{ 0.0f }
	, m_bounds{ }
	, m_density{ 1.0f }
	, m_friction{ 0.4f }
	, m_frictionCombine{ PhysicsMaterialCombine2D::Mean }
	, m_isTrigger{ false }
	, m_offset{ 0.0f, 0.0f }
	, m_sharedMaterial{ }
	, m_callbackLayers{ -1 }
	, m_contactCaptureLayers{ -1 }
	, m_contactMask{ -1 }
	, m_excludeLayers{ 0 }
	, m_forceReceiveLayers{ -1 }
	, m_forceSendLayers{ -1 }
	, m_includeLayers{ 0 }
	, m_pComposite{ nullptr }
	, m_compositeCapable{ false }
	, m_compositeOperation{ CompositeOperation::None }
	, m_compositeOrder{ 0 }
	, m_errorState{ ColliderErrorState2D::None }
	, m_layerOverridePriority{ 0 }
	, m_shapeCount{ 0 }

{}

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::Collider2D::GetTypeName() const
{
    static const std::string typeName = "Collider2D";
    return typeName;
}

/*---------------------------------------------------------------------------------------
| --- CanContact:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::CanContact(const Collider2D* pCollider) const
{
    return false;
}

/*---------------------------------------------------------------------------------------
| --- ClosestPoint:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Collider2D::ClosestPoint(const Vector2f& position) const
{
    return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- Distance:
---------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::Collider2D::Distance(const Collider2D* pCollider) const
{
    return ColliderDistance2D();
}

/*---------------------------------------------------------------------------------------
| --- IsTouching:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const Collider2D* pCollider) const
{
    return false;
}

/*---------------------------------------------------------------------------------------
| --- IsTouching:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
    return false;
}

/*---------------------------------------------------------------------------------------
| --- IsTouching:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const ContactFilter2D& contactFilter) const
{
    return false;
}

/*---------------------------------------------------------------------------------------
| --- IsTouchingLayers:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouchingLayers(LayerMask) const
{
    return false;
}

/*---------------------------------------------------------------------------------------
| --- OverlapPoint:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::OverlapPoint(const Vector2f& point) const
{
    return false;
}

/*---------------------------------------------------------------------------------------
| --- OnCollisionEnter2D:
---------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionEnter2D(const Collision2D& collision)
{
}

/*---------------------------------------------------------------------------------------
| --- OnCollisionExit2D:
---------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionExit2D(const Collision2D& collision)
{
}

/*---------------------------------------------------------------------------------------
| --- OnCollisionStay2D:
---------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionStay2D(const Collision2D& collision)
{
}

/*---------------------------------------------------------------------------------------
| --- OnTriggerEnter2D:
---------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerEnter2D(const Collision2D& collision)
{
}

/*---------------------------------------------------------------------------------------
| --- OnTriggerExit2D:
---------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerExit2D(const Collision2D& collision)
{
}

/*---------------------------------------------------------------------------------------
| --- OnTriggerStay2D:
---------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerStay2D(const Collision2D& collision)
{
}

/*---------------------------------------------------------------------------------------
| --- SetSharedMaterial:
---------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::SetSharedMaterial(const PhysicsMaterial2D& material)
{
}

/*---------------------------------------------------------------------------------------
| --- RefreshBounds:
---------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::RefreshBounds()
{
}

/*---------------------------------------------------------------------------------------
| --- GetShapes:
---------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetShapes(PhysicsShapeGroup2D& physicsSHapeGroup) const
{
    return 0;
}

/*---------------------------------------------------------------------------------------
| --- GetShapesHash:
---------------------------------------------------------------------------------------*/
std::uint32_t CaledonEngine::Collider2D::GetShapesHash() const
{
    return std::uint32_t();
}

/*---------------------------------------------------------------------------------------
| --- GetShapeBounds:
---------------------------------------------------------------------------------------*/
CaledonEngine::AABB2D CaledonEngine::Collider2D::GetShapeBounds(std::vector<AABB2D>& bounds, bool useRadii, bool useWorldSpace) const
{
    return AABB2D();
}

/*---------------------------------------------------------------------------------------
| --- Cast:
---------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::Cast(const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance, bool ignoreSiblingColliders) const
{
    return 0;
}

/*---------------------------------------------------------------------------------------
| --- Overlap:
---------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::Overlap(const Vector2f& position, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
    return 0;
}

/*---------------------------------------------------------------------------------------
| --- Raycast:
---------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::Raycast(const Vector2f& direction, std::span<ColliderHit2D> results, float distance, LayerMask layerMask, float minDepth, float maxDepth) const
{
    return 0;
}

/*---------------------------------------------------------------------------------------
| --- GetContacts:
---------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
    return 0;
}

/*---------------------------------------------------------------------------------------
| --- GetContactColliders:
---------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const
{
    return 0;
}