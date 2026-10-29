/*------------------------------
| File: PhysicsScene2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "PhysicsScene2D.h"

#include "Systems/Physics/Components/RigidBody2D.h"

#include <algorithm>

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
	if (pCollider == nullptr || !m_isValid)
		return false;

	if (std::find(m_registeredColliders.begin(), m_registeredColliders.end(), pCollider) != m_registeredColliders.end())
		return false;

	m_registeredColliders.push_back(pCollider);
	return true;
}

/*-----------------------------------------------------------------------
| --- RemoveCollider: Removes a Collider2D from this PhysicsScene2D --- |
-----------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::RemoveCollider(Collider2D* pCollider)
{
	const auto it = std::find(m_registeredColliders.begin(), m_registeredColliders.end(), pCollider);
	if (it == m_registeredColliders.end())
		return false;

	m_registeredColliders.erase(it);		// Order-preserving: the simulation order stays deterministic

	// A removed collider must not leave dangling pointers in the ignore list
	m_ignoredPairs.erase(std::remove_if(m_ignoredPairs.begin(), m_ignoredPairs.end(),
		[pCollider](const auto& pair) { return pair.first == pCollider || pair.second == pCollider; }), m_ignoredPairs.end());

	return true;
}

/*-------------------------------------------------------------------
| --- AddRigidBody2D: Adds a RigidBody2D to this PhysicsScene2D --- |
-------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::AddRigidBody2D(RigidBody2D* pRigidBody2D)
{
	if (pRigidBody2D == nullptr || !m_isValid)
		return false;

	if (std::find(m_registeredBodies.begin(), m_registeredBodies.end(), pRigidBody2D) != m_registeredBodies.end())
		return false;

	m_registeredBodies.push_back(pRigidBody2D);
	return true;
}

/*---------------------------------------------------------------------------
| --- RemoveRigidBody2D: Removes a RigidBody2D from this PhysicsScene2D --- |
---------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::RemoveRigidBody2D(RigidBody2D* pRigidBody2D)
{
	const auto it = std::find(m_registeredBodies.begin(), m_registeredBodies.end(), pRigidBody2D);
	if (it == m_registeredBodies.end())
		return false;

	m_registeredBodies.erase(it);			// Order-preserving: the simulation order stays deterministic
	return true;

}

/*----------------------------------------------------------------------------------
| --- WakeAllBodies: Wakes every registered RigidBody2D in this PhysicsScene2D --- |
----------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsScene2D::WakeAllBodies() noexcept
{
	for (RigidBody2D* pBody : m_registeredBodies)
	{
		pBody->WakeUp();
	}
}

/*-----------------------------------------------------------------------------------------------
| --- GetIgnoreCollision: Checks whether collision between two Collider2D are to be ignored --- |
-----------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::GetIgnoreCollision(const Collider2D* pCollider1, const Collider2D* pCollider2) const
{
	auto* pFirst = const_cast<Collider2D*>(pCollider1);
	auto* pSecond = const_cast<Collider2D*>(pCollider2);

	if (std::less<Collider2D*>{}(pSecond, pFirst))
	{
		std::swap(pFirst, pSecond);
	}

	return std::find(m_ignoredPairs.begin(), m_ignoredPairs.end(), std::make_pair(pFirst, pSecond)) != m_ignoredPairs.end();
}

/*--------------------------------------------------------------------------------------------
| --- SetIgnoreCollision: Sets a collision between two Collider2D to be ignored (or not) --- |
--------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsScene2D::SetIgnoreCollision(Collider2D* pCollider1, Collider2D* pCollider2, bool ignore)
{
	if (pCollider1 == nullptr || pCollider2 == nullptr || pCollider1 == pCollider2)
		return;

	if (std::less<Collider2D*>{}(pCollider2, pCollider1))
	{
		std::swap(pCollider1, pCollider2);
	}

	const auto it = std::find(m_ignoredPairs.begin(), m_ignoredPairs.end(), std::make_pair(pCollider1, pCollider2));

	if (ignore && it == m_ignoredPairs.end())
	{
		m_ignoredPairs.emplace_back(pCollider1, pCollider2);
	}
	else if (!ignore && it != m_ignoredPairs.end())
	{
		m_ignoredPairs.erase(it);
	}
}

/*---------------------------------------------------------------------------------------------------------
| --- BoxCast: Returns all intersections of a casted Box against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return std::vector<ColliderHit2D>();
}

/*---------------------------------------------------------------------------------------------------------
| --- BoxCast: Returns all intersections of a casted Box against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return 0;
}

/*--------------------------------------------------------------------------------------------------------------
| --- BoxCastAll: Returns all of the unique Colliders intersected by the casted Box in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::BoxCastAll(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
}

/*-----------------------------------------------------------------------------------------------------------------
| --- CapsuleCast: Returns all intersections of a casted Capsule against the Colliders in this PhysicsScene2D --- |
-----------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return std::vector<ColliderHit2D>();
}

/*-----------------------------------------------------------------------------------------------------------------
| --- CapsuleCast: Returns all intersections of a casted Capsule against the Colliders in this PhysicsScene2D --- |
-----------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return 0;
}

/*----------------------------------------------------------------------------------------------------------------------
| --- CapsuleCastAll: Returns all of the unique Colliders intersected by the casted Capsule in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::CapsuleCastAll(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
}

/*---------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return std::vector<ColliderHit2D>();
}

/*---------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------
| --- CircleCastAll: Returns all of the unique Colliders intersected by the casted Circle in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::CircleCastAll(const Vector2f& origin, float radius, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
}

/*---------------------------------------------------------------------------------------------------------
| --- Raycast: Returns all intersections of a casted Ray against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return std::vector<ColliderHit2D>();
}

/*---------------------------------------------------------------------------------------------------------
| --- Raycast: Returns all intersections of a casted Ray against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return 0;
}

/*--------------------------------------------------------------------------------------------------------------
| --- RaycastAll: Returns all of the unique Colliders intersected by the casted Ray in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::RaycastAll(const Vector2f& origin, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
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

/*------------------------------------------------------------------------------------------------------------------------
| --- LinecastAll: Returns all of the unique Colliders intersected by the casted Line segment in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::LinecastAll(const Vector2f& start, const Vector2f& end, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
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

/*------------------------------------------------------------------------------------------------------------------------
| --- GetRayIntersectionAll: Returns all of the unique Colliders interesected against a Ray2D in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::GetRayIntersectionAll(const Ray2D& ray, float distance, int layerMask) const
{
	return std::vector<ColliderHit2D>();
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

/*------------------------------------------------------------------------------------------------------------------------
| --- OverlapAreaAll: Returns all of the unique Colliders intersected by the overlapping Area in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapAreaAll(const Vector2f& pointA, const Vector2f& pointB, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
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

/*----------------------------------------------------------------------------------------------------------------------
| --- OverlapBoxAll: Returns all of the unique Colliders intersected by the overlapping Box in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapBoxAll(const Vector2f& point, const Vector2f& size, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
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

/*------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsuleAll: Returns all of the unique Colliders intersected by the overlapping Capsule in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapCapsuleAll(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
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
| --- OverlapCircleAll: Returns all of the unique Colliders intersected by the overlapping Circle in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapCircleAll(const Vector2f& point, float radius, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
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

/*--------------------------------------------------------------------------------------------------------------------------
| --- OverlapPointAll: Returns all of the unique Colliders intersected by the overlapping Point in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapPointAll(const Vector2f& point, int layerMask, float minDepth, float maxDepth) const
{
	return std::vector<Collider2D*>();
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- ClosestPoint:
--------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::PhysicsScene2D::ClosestPoint(const Vector2f& position, const Collider2D* pCollider) const
{
	if (pCollider == nullptr)
		return position;

	// TODO: Implement shape-to-point closest distance calculation against pCollider's geometry
	return position;
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- Distance:
--------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::PhysicsScene2D::Distance(const Collider2D* pColliderA, const Collider2D* pColliderB) const
{
	if (pColliderA == nullptr || pColliderB == nullptr)
		return ColliderDistance2D{};

	// TODO: Implement narrowphase distance/separation query between pColliderA and pColliderB
	return ColliderDistance2D{};
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- IsTouching:
--------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::IsTouching(const Collider2D* pCollider1, const Collider2D* pCollider2) const
{
	if (pCollider1 == nullptr || pCollider2 == nullptr)
		return false;

	// TODO: Check if pCollider1 and pCollider2 have an active contact manifold
	return false;
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- IsTouching:
--------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	if (pCollider == nullptr)
		return false;

	std::vector<Collider2D*> results;
	OverlapCollider(pCollider, contactFilter, results);
	return !results.empty();
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- IsTouching:
--------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::IsTouching(const ContactFilter2D& contactFilter) const
{
	// TODO: Implement -> the contact filter is used to filter the results differently, such as by layer mask, Z depth, or normal angle
	//					  returns whether the Collider is touching any other Collider filtered by the contactFilter or not
	return false;
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- IsTouchingLayers:
--------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::IsTouchingLayers(LayerMask layerMask) const
{
	ContactFilter2D filter = ContactFilter2D::NoFilter();
	filter.SetLayerMask(layerMask);
	return IsTouching(filter);
}

/*-----------------------------------------------------------------------------------------------------
| --- SyncTransforms: Pushes the Transform poses of all registered bodies into the physics bodies --- |
-----------------------------------------------------------------------------------------------------*/
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