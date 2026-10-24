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

/*-------------------------------------------------------
| --- Initialize: Prepares this RigidBody2D for use --- |
-------------------------------------------------------*/
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

/*-------------------------------------------------------------
| --- SetBodyType: Sets the BodyType for this RigidBody2D --- |
-------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetBodyType(BodyType2D type) noexcept
{
}

/*---------------------------------------------------------------------------------
| --- SetFreezeRotation: Sets whether rotation is frozen for this RigidBody2D --- |
---------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetFreezeRotation(bool freeze) noexcept
{
}

/*------------------------------------------------------------------------------
| --- Sleep: Make this RigidBody2D 'sleep' (ignored for NeverSleep bodies) --- |
------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::Sleep()
{
}

/*-------------------------------------------------------------------------------
| --- SetRotation: Sets the rotation of this RigidBody2D to the given angle --- |
-------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetRotation(float angle)
{
}

/*--------------------------------------------------------------------
| --- MovePosition: Moves this RigidBody2D to the given position --- |
--------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MovePosition(const Vector2f& position)
{
}

/*--------------------------------------------------------------------------------------------------
| --- MovePositionAndRotation: Moves this RigidBody2D to the given position and rotation angle --- |
--------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MovePositionAndRotation(const Vector2f& position, float angle)
{
}

/*--------------------------------------------------------------------------
| --- MoveRotation: Moves this RigidBody2D to the given rotation angle --- |
--------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MoveRotation(float angle)
{
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- Slide: Slide this RigidBody2D using the given velocity integrated over deltaTime using the specified SlideConfig --- |
--------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::SlideResults2D CaledonEngine::RigidBody2D::Slide(const Vector2f& velocity, float deltaTime, const SlideConfig2D& slideConfig)
{
	return SlideResults2D();
}

/*----------------------------------------------------------------------------------------------------------------------------
| --- SyncFromTransform: Copies the owner's Transform pose into the body (called by the physics subsystem before a step) --- |
----------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SyncFromTransform()
{
}

/*-----------------------------------------------------------------------------------------------------------------------------
| --- ApplyToTransform: Writes the body pose back to the owner's Transform (called by the physics subsystem after a step) --- |
-----------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::ApplyToTransform() const
{
}

/*-------------------------------------------------------------------------
| --- SetLinearVelocity: Sets the linear velocity of this RigidBody2D --- |
-------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetLinearVelocity(const Vector2f& velocity) noexcept
{
}

/*---------------------------------------------------------------------------
| --- SetAngularVelocity: Sets the angular velocity of this RigidBody2D --- |
---------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetAngularVelocity(float velocity) noexcept
{
}

/*-----------------------------------------------------
| --- AddForce: Apply a force to this RigidBody2D --- |
-----------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForce(const Vector2f& force, ForceMode2D mode)
{
}

/*------------------------------------------------------------------------------
| --- AddForceAtPosition: Apply a force at a given position in world-space --- |
------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceAtPosition(const Vector2f& force, const Vector2f& position, ForceMode2D mode)
{
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- AddForceX: Adds a force to the X component of the linear velocity only leaving the Y component of the world-space untouched --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceX(float force, ForceMode2D mode)
{
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- AddForceY: Adds a force to the Y component of the linear velocity only leaving the X component of the world-space untouched --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceY(float force, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------
| --- AddRelativeForce: Adds a force to the local-space linear velocity --- |
---------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForce(const Vector2f& relativeForce, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
| --- AddRelativeForceX: Adds a force to the X component of the linear velocity only leaving the Y component of the local-space untouched --- |
---------------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForceX(float force, ForceMode2D mode)
{
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
| --- AddRelativeForceY: Adds a force to the Y component of the linear velocity only leaving the X component of the local-space untouched --- |
---------------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForceY(float force, ForceMode2D mode)
{
}

/*------------------------------------------------------------------------
| --- AddTorque: Apply a torque at this RigidBody2D's center of mass --- |
------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddTorque(float torque, ForceMode2D mode)
{
}

/*-----------------------------------------------------------------------------------------------
| --- SetUseAutoMass: Sets whether mass is automatically calculated from attached colliders --- |
-----------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetUseAutoMass(bool useAutoMass)
{
}

/*----------------------------------------------------
| --- SetMass: Sets the mass of this RigidBody2D --- |
----------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetMass(float mass) noexcept
{
}

/*-------------------------------------------------------------------------------------------
| --- GetWorldCenterOfMass: Returns the world-space center of mass for this RigidBody2D --- |
-------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetWorldCenterOfMass() const noexcept
{
	return Vector2f();
}

/*------------------------------------------------------------------------------------------------------------------------------
| --- UpdateMassData: Computes the mass, center of mass, and inertia from the attached colliders (when useAutoMass = true) --- |
------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::UpdateMassData()
{
}

/*-----------------------------------------------------------------------------------------
| --- GetPoint: Returns a local space point given the point in rigidBody global space --- |
-----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetPoint(const Vector2f& point) const
{
	return Vector2f();
}

/*--------------------------------------------------------------------------------------------
| --- GetVector: Returns a local space vector given the vector in rigidBody global space --- |
--------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetVector(const Vector2f& vector) const
{
	return Vector2f();
}

/*---------------------------------------------------------------------------------------------------------------
| --- GetRelativePoint: Returns a global space point given the point relativePoint in rigidBody local space --- |
---------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativePoint(const Vector2f& relativePoint) const
{
	return Vector2f();
}

/*-------------------------------------------------------------------------------------------------------------------
| --- GetRelativeVector: Returns a global space vector given the vector relativeVector in rigidBody local space --- |
-------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativeVector(const Vector2f& relativeVector) const
{
	return Vector2f();
}

/*-------------------------------------------------------------------------------------
| --- GetPointVelocity: The velocity of the RigidBody2D at a point in world space --- |
-------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetPointVelocity(const Vector2f& point) const
{
	return Vector2f();
}

/*-------------------------------------------------------------------------------------------------
| --- GetRelativePointVelocity: The velocity of the RigidBody2D at a point in its local space --- |
-------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativePointVelocity(const Vector2f& relativePoint) const
{
	return Vector2f();
}

/*--------------------------------------------------------------------------------------------
| --- GetAttachedColliders: Returns all Collider2D that are attached to this RigidBody2D --- |
--------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetAttachedColliders(std::vector<Collider2D*>& results, bool findTriggers) const
{
	return 0;
}

/*------------------------------------------------------------------------------------------------
| --- RefreshAttachedColliders: Returns all Collider2D that are attached to this RigidBody2D --- |
------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::RefreshAttachedColliders()
{
}

/*---------------------------------------------------------------------------
| --- AttachCollider: Attaches a Collider2D; returns false if it failed --- |
---------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::AttachCollider(Collider2D* pCollider)
{
	return false;
}

/*---------------------------------------------------------------------------
| --- DetachCollider: Detaches a Collider2D; returns false if it failed --- |
---------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::DetachCollider(Collider2D* pCollider)
{
	return false;
}

/*-----------------------------------------------------------------------------
| --- SetSharedMaterial: Applies the material to every attached Collider2 --- |
-----------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetSharedMaterial(const PhysicsMaterial2D& material)
{
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------
| --- ClosestPoint: Returns a point on the perimeter of all enabled Colliders attached to this RigidBody that is closest to the specified position --- |
------------------------------------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::ClosestPoint(const Vector2f& position) const
{
	return Vector2f();
}

/*------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Check if any of the RigidBody2D colliders overlap a given point in world-space --- |
------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::OverlapPoint(const Vector2f& point) const
{
	return false;
}

/*-------------------------------------------------------------------------------------------------------
| --- GetShapes: Returns all the PhysicsShape2D used by all Collider2D attached to this RigidBody2D --- |
-------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetShapes(PhysicsShapeGroup2D& physicsShapeGroup) const
{
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------------
| --- Distance: Calculates the minimum distance of this collider against all Collider2D attached to this RigidBody2D --- |
------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::RigidBody2D::Distance(const Collider2D* pCollider) const
{
	return ColliderDistance2D();
}

/*------------------------------------------------------------------------------------------------------------------------
| --- IsTouching: Checks whether the collider is touching any of the collider(s) attached to this RigidBody2D or not --- |
------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsTouching(const Collider2D* pCollider) const
{
	return false;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------
| --- IsTouchingLayers: Checks whether any of the collider(s) attached to this RigidBody2D are touching any colliders on the specified layerMask or not --- |
-----------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsTouchingLayers(LayerMask layerMask) const
{
	return false;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| --- Cast: All the Collider2D shapes attached to this RigidBody2D are cast into the Scene starting at each Collider position ignoring the Colliders attached to the same RigidBody2D --- |
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::Cast(const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return 0;
}

/*----------------------------------------------------------------------------------------------------------
| --- Overlap: Returns a list of all Colliders that overlap all Colliders attached to this RigidBody2D --- |
----------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::Overlap(const Vector2f& position, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------------------------
| --- GetContacts: Returns all contact points for all of the Collider(s) attached to this RigidBody2D --- |
---------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
	return 0;
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- GetContactColliders: Returns all colliders in contact with this RigidBody2D, with the results filtered by the contactFilter --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const
{
	return 0;
}