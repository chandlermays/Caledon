/*------------------------------
| File: PhysicsScene2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "PhysicsScene2D.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------
| --- Constructor: Constructs this PhysicsScene2D with default values --- |
-------------------------------------------------------------------------*/
CaledonEngine::PhysicsScene2D::PhysicsScene2D(const Physics2DSettings& settings)
	: m_registeredColliders{ }
	, m_registeredBodies{ }
	, m_pSettings{ &settings }
	, m_subStepCount{ 0 }
	, m_subStepLostTime{ 0.0f }
	, m_isValid{ true }
{ }


/*---------------------------------------------------------------
| --- AddCollider: Adds a Collider2D to this PhysicsScene2D --- |
---------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::AddCollider(Collider2D* pCollider)
{
	return false;
}

/*-----------------------------------------------------------------------
| --- RemoveCollider: Removes a Collider2D from this PhysicsScene2D --- |
-----------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::RemoveCollider(Collider2D* pCollider)
{
	return false;
}

/*-------------------------------------------------------------------
| --- AddRigidBody2D: Adds a RigidBody2D to this PhysicsScene2D --- |
-------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::AddRigidBody2D(RigidBody2D* pRigidBody2D)
{
	return false;
}

/*---------------------------------------------------------------------------
| --- RemoveRigidBody2D: Removes a RigidBody2D from this PhysicsScene2D --- |
---------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::RemoveRigidBody2D(RigidBody2D* pRigidBody2D)
{
	return false;
}

/*---------------------------------------------------------------------------------------------------------
| --- BoxCast: Returns all intersections of a casted Box against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter) const
{
	return std::vector<ColliderHit2D>();
}

/*---------------------------------------------------------------------------------------------------------
| --- BoxCast: Returns all intersections of a casted Box against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::BoxCast(const Vector2f& origin, const Vector2f& sie, float angle, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return 0;
}

/*-----------------------------------------------------------------------------------------------------------------
| --- CapsuleCast: Returns all intersections of a casted Capsule against the Colliders in this PhysicsScene2D --- |
-----------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter) const
{
	return std::vector<ColliderHit2D>();
}

/*-----------------------------------------------------------------------------------------------------------------
| --- CapsuleCast: Returns all intersections of a casted Capsule against the Colliders in this PhysicsScene2D --- |
-----------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, float distance, ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter) const
{
	return std::vector<ColliderHit2D>();
}

/*---------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------------------------
| --- RayCast: Returns all intersections of a casted Ray against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::Raycast(const Vector2f& origin, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter) const
{
	return std::vector<ColliderHit2D>();
}

/*---------------------------------------------------------------------------------------------------------
| --- RayCast: Returns all intersections of a casted Ray against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::Raycast(const Vector2f& origin, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return 0;
}

/*-------------------------------------------------------------------------------------------------------------------
| --- Linecast: Returns all intersections of a casted Line segment against the Colliders in this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter) const
{
	return std::vector<ColliderHit2D>();
}

/*-------------------------------------------------------------------------------------------------------------------
| --- Linecast: Returns all intersections of a casted Line segment against the Colliders in this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
| --- GetRayIntersection: Returns all intersections of a Ray2D against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::GetRayIntersection(const Ray2D& ray, float distance, int layerMask) const
{
	return std::vector<ColliderHit2D>();
}

/*---------------------------------------------------------------------------------------------------------------
| --- GetRayIntersection: Returns all intersections of a Ray2D against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::GetRayIntersection(const Ray2D& ray, float distance, int layerMask, std::span<ColliderHit2D> results) const
{
	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------
| --- OverlapArea: Returns all intersections of an overlapping Area against the Colliders in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter) const
{
	return std::vector<Collider2D*>();
}

/*--------------------------------------------------------------------------------------------------------------------
| --- OverlapArea: Returns all intersections of an overlapping Area against the Colliders in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
| --- OverlapBox: Returns all intersections of an overlapping Box against the Colliders in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter) const
{
	return std::vector<Collider2D*>();
}

/*------------------------------------------------------------------------------------------------------------------
| --- OverlapBox: Returns all intersections of an overlapping Box against the Colliders in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Colliders in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter) const
{
	return std::vector<Collider2D*>();
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Colliders in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Colliders in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter) const
{
	return std::vector<Collider2D*>();
}

/*------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Colliders in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return 0;
}

/*----------------------------------------------------------------------------------------------------------------------------
| --- OverlapCollider: Returns all intersections of an overlapping Collider against the Colliders in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	return std::vector<Collider2D*>();
}

/*----------------------------------------------------------------------------------------------------------------------------
| --- OverlapCollider: Returns all intersections of an overlapping Collider against the Colliders in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return 0;
}

/*----------------------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Returns all intersections of an overlapping Point against the Colliders in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter) const
{
	return std::vector<Collider2D*>();
}

/*----------------------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Returns all intersections of an overlapping Point against the Colliders in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return 0;
}

/*----------------------------------------------------------------------------------------------------
| --- SyncTransform: Pushes the Transform poses of all registered bodies into the physics bodies --- |
----------------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsScene2D::SyncTransforms()
{
}

/*------------------------------------------------------------------------
| --- Simulate: Simulate physics associated with this PhysicsScene2D --- |
------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::Simulate(float deltaTime, int simulationLayers)
{
	return false;
}