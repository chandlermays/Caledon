/*------------------------------
| File: RigidBody2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "RigidBody2D.h"

#include "Systems/Physics/Components/Collider2D.h"

/*---------------------------------------------------------------------------------------
| --- WakeIfNeeded:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::WakeIfNeeded() noexcept
{
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------
| --- Constructor: Constructs this RigidBody2D with default values --- |
----------------------------------------------------------------------*/
CaledonEngine::RigidBody2D::RigidBody2D()
{}

/*---------------------------------------------------------------------------------------
| --- Initialize:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::Initialize()
{
	return false;
}

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::RigidBody2D::GetTypeName() const
{
	static const std::string typeName = "RigidBody2D";
	return typeName;
}

/*---------------------------------------------------------------------------------------
| --- SetBodyType:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetBodyType(BodyType2D type) noexcept
{
}

/*---------------------------------------------------------------------------------------
| --- SetFreezeRotation:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetFreezeRotation(bool freeze) noexcept
{
}

/*---------------------------------------------------------------------------------------
| --- Sleep:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::Sleep()
{
}

/*---------------------------------------------------------------------------------------
| --- SetRotation:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetRotation(float angle)
{
}

/*---------------------------------------------------------------------------------------
| --- MovePosition:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MovePosition(const Vector2f& position)
{
}

/*---------------------------------------------------------------------------------------
| --- MovePositionAndRotation:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MovePositionAndRotation(const Vector2f& position, float angle)
{
}

/*---------------------------------------------------------------------------------------
| --- MoveRotation:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MoveRotation(float angle)
{
}

/*---------------------------------------------------------------------------------------
| --- Slide:
---------------------------------------------------------------------------------------*/
CaledonEngine::SlideResults2D CaledonEngine::RigidBody2D::Slide(const Vector2f& velocity, float deltaTime, const SlideConfig2D& slideConfig)
{
	return SlideResults2D();
}

/*---------------------------------------------------------------------------------------
| --- SyncFromTransform:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SyncFromTransform()
{
}

/*---------------------------------------------------------------------------------------
| --- ApplyToTransform:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::ApplyToTransform() const
{
}

/*---------------------------------------------------------------------------------------
| --- SetLinearVelocity:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetLinearVelocity(const Vector2f& velocity) noexcept
{
}

/*---------------------------------------------------------------------------------------
| --- SetAngularVelocity:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetAngularVelocity(float velocity) noexcept
{
}

/*---------------------------------------------------------------------------------------
| --- AddForce:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForce(const Vector2f& force, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------
| --- AddForceAtPosition:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceAtPosition(const Vector2f& force, const Vector2f& position, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------
| --- AddForceX:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceX(float force, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------
| --- AddForceY:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceY(float force, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------
| --- AddRelativeForce:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForce(const Vector2f& relativeForce, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------
| --- AddRelativeForceX:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForceX(float force, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------
| --- AddRelativeForceY:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForceY(float force, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------
| --- AddTorque:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddTorque(float torque, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------
| --- SetUseAutoMass:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetUseAutoMass(bool useAutoMass)
{
}

/*---------------------------------------------------------------------------------------
| --- SetMass:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetMass(float mass) noexcept
{
}

/*---------------------------------------------------------------------------------------
| --- GetWorldCenterOfMass:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetWorldCenterOfMass() const noexcept
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- UpdateMassData:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::UpdateMassData()
{
}

/*---------------------------------------------------------------------------------------
| --- GetPoint:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetPoint(const Vector2f& point) const
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- GetVector:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetVector(const Vector2f& vector) const
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- GetRelativePoint:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativePoint(const Vector2f& relativePoint) const
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- GetRelativeVector:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativeVector(const Vector2f& relativeVector) const
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- GetPointVelocity:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetPointVelocity(const Vector2f& point) const
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- GetRelativePointVelocity:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativePointVelocity(const Vector2f& relativePoint) const
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- GetAttachedColliders:
---------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetAttachedColliders(std::vector<Collider2D*>& results, bool findTriggers) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------
| --- RefreshAttachedColliders:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::RefreshAttachedColliders()
{
}

/*---------------------------------------------------------------------------------------
| --- AttachCollider:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::AttachCollider(Collider2D* pCollider)
{
	return false;
}

/*---------------------------------------------------------------------------------------
| --- DetachCollider:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::DetachCollider(Collider2D* pCollider)
{
	return false;
}

/*---------------------------------------------------------------------------------------
| --- SetSharedMaterial:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetSharedMaterial(const PhysicsMaterial2D& material)
{
}

/*---------------------------------------------------------------------------------------
| --- ClosestPoint:
---------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::ClosestPoint(const Vector2f& position) const
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------
| --- OverlapPoint:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::OverlapPoint(const Vector2f& point) const
{
	return false;
}

/*---------------------------------------------------------------------------------------
| --- GetShapes:
---------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetShapes(PhysicsShapeGroup2D& physicsShapeGroup) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------
| --- Distance:
---------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::RigidBody2D::Distance(const Collider2D* pCollider) const
{
	return ColliderDistance2D();
}

/*---------------------------------------------------------------------------------------
| --- IsTouching:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsTouching(const Collider2D* pCollider) const
{
	return false;
}

/*---------------------------------------------------------------------------------------
| --- IsTouchingLayers:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsTouchingLayers(LayerMask layerMask) const
{
	return false;
}

/*---------------------------------------------------------------------------------------
| --- Cast:
---------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::Cast(const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------
| --- Overlap:
---------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::Overlap(const Vector2f& position, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------
| --- GetContacts:
---------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------
| --- GetContactColliders:
---------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const
{
	return 0;
}