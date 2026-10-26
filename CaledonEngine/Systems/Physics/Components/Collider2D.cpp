/*------------------------------
| File: Collider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "Collider2D.h"

#include "Systems/Physics/Components/RigidBody2D.h"
#include "Systems/Physics/PhysicsScene2D.h"
#include "Systems/Scene/Scene.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------
| --- Constructor: Constructs this Collider2D with default values --- |
---------------------------------------------------------------------*/
CaledonEngine::Collider2D::Collider2D()
	: m_pAttachedRigidBody{ nullptr }
	, m_pPhysicsScene2D{ nullptr }
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

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::Collider2D::~Collider2D()
{
	if (m_pPhysicsScene2D != nullptr)
	{
		m_pAttachedRigidBody->DetachCollider(this);
		m_pPhysicsScene2D->RemoveCollider(this);
	}
}

/*------------------------------------------------------
| --- Initialize: Prepares this Collider2D for use --- |
------------------------------------------------------*/
bool CaledonEngine::Collider2D::Initialize()
{
	if (m_pPhysicsScene2D != nullptr)
		return true;

	const Scene* pScene = (m_pOwner != nullptr) ? m_pOwner->GetScene() : nullptr;
	m_pPhysicsScene2D = (pScene != nullptr) ? pScene->GetPhysicsScene2D() : nullptr;
	if (m_pPhysicsScene2D == nullptr)
		return false;

	if (!m_pPhysicsScene2D->AddCollider(this))
		return false;

	return true;
}

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::Collider2D::GetTypeName() const
{
    static const std::string typeName = "Collider2D";
    return typeName;
}

/*------------------------------------------------------------------------------
| --- CanContact: Determines if both Collider2D can ever come into contact --- |
------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::CanContact(const Collider2D* pCollider) const
{
    return false;
}

/*-----------------------------------------------------------------------------------------------------------------
| --- ClosestPoint: Returns a point on the perimeter of this Collider2D that is closest to the given position --- |
-----------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Collider2D::ClosestPoint(const Vector2f& position) const
{
    return Vector2f();
}

/*---------------------------------------------------------------------------------------------------
| --- Distance: Calculates the minimum seperation of this Collider2D against another Collider2D --- |
---------------------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::Collider2D::Distance(const Collider2D* pCollider) const
{
    return ColliderDistance2D();
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: Check whether this Collider2D is touching the Collider2D or not --- |
-------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const Collider2D* pCollider) const
{
    return false;
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: Check whether this Collider2D is touching the Collider2D or not --- |
-------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
    return false;
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: Check whether this Collider2D is touching the Collider2D or not --- |
-------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const ContactFilter2D& contactFilter) const
{
    return false;
}

/*---------------------------------------------------------------------------------------------------------------------
| --- IsTouchingLayers: Check whether this Collider2D is touching any colliders on the specified LayerMask or not --- |
---------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouchingLayers(LayerMask) const
{
    return false;
}

/*-----------------------------------------------------------------------------------
| --- OverlapPoint: Check if a Collider2D overlaps a given point in world-space --- |
-----------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::OverlapPoint(const Vector2f& point) const
{
    return false;
}

/*------------------------------------------------------------------------------------------------------
| --- OnCollisionEnter2D: Sent when an incoming collider makes contact with this object's collider --- |
------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionEnter2D(const Collision2D& collision)
{
}

/*---------------------------------------------------------------------------------------------------------
| --- OnCollisionExit2D: Sent when a collider on another object stops touching this object's collider --- |
---------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionExit2D(const Collision2D& collision)
{
}

/*------------------------------------------------------------------------------------------------------------------
| --- OnCollisionStay2D: Sent each frame where a collider on another object is touching this object's collider --- |
------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionStay2D(const Collision2D& collision)
{
}

/*------------------------------------------------------------------------------------------------------
| --- OnTriggerEnter2D: Sent when another object enters a trigger collider attached to this object --- |
------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerEnter2D(const Collision2D& collision)
{
}

/*-----------------------------------------------------------------------------------------------------
| --- OnTriggerExit2D: Sent when another object leaves a trigger collider attached to this object --- |
-----------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerExit2D(const Collision2D& collision)
{
}

/*--------------------------------------------------------------------------------------------------------------------
| --- OnTriggerStay2D: Sent each frame where another object is within a trigger collider attached to this object --- |
--------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerStay2D(const Collision2D& collision)
{
}

/*--------------------------------------------------------------------------------
| --- SetSharedMaterial: Sets the shared physics material of this Collider2D --- |
--------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::SetSharedMaterial(const PhysicsMaterial2D& material)
{
}

/*-----------------------------------------------------------------------
| --- RefreshBounds: Refreshes the bounding area of this Collider2D --- |
-----------------------------------------------------------------------*/
void CaledonEngine::Collider2D::RefreshBounds()
{
}

/*------------------------------------------------------------------
| --- GetShapes: Populates the shape group with physics shapes --- |
------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetShapes(PhysicsShapeGroup2D& physicsSHapeGroup) const
{
    return 0;
}

/*-------------------------------------------------------------------------
| --- GetShapesHash: Generates a hash representing the current shapes --- |
-------------------------------------------------------------------------*/
std::uint32_t CaledonEngine::Collider2D::GetShapesHash() const
{
    return std::uint32_t();
}

/*-------------------------------------------------------------------
| --- GetShapeBounds: Returns bounds for all constituent shapes --- |
-------------------------------------------------------------------*/
CaledonEngine::AABB2D CaledonEngine::Collider2D::GetShapeBounds(std::vector<AABB2D>& bounds, bool useRadii, bool useWorldSpace) const
{
    return AABB2D();
}

/*-------------------------------------------------------------------------------------
| --- Cast: 
-------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::Cast(const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance, bool ignoreSiblingColliders) const
{
    return 0;
}

/*----------------------------------------------------------------------------------------
| --- Overlap: 
----------------------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------
| --- GetContacts: Retrieves contact points involving this Collider2D --- |
-------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
    return 0;
}

/*--------------------------------------------------------------------------------------------
| --- GetContactColliders: Retrieves colliders currently in contact with this Collider2D --- |
--------------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const
{
    return 0;
}