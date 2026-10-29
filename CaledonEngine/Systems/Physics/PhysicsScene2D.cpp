/*------------------------------
| File: PhysicsScene2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "PhysicsScene2D.h"

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/Components/RigidBody2D.h"
#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"
#include "Systems/Scene/GameObject.h"

#include <algorithm>
#include <cmath>

namespace
{
	/*----------------------------------------------------------------------------
	| --- PassesFilter:
	----------------------------------------------------------------------------*/
	bool PassesFilter(const CaledonEngine::Collider2D& collider, const CaledonEngine::ContactFilter2D& contactFilter)
	{
		const CaledonEngine::GameObject* pOwner = collider.GetOwner();
		if (!collider.IsActive() || pOwner == nullptr || !pOwner->IsActive())
			return false;

		if (contactFilter.IsFilteringTrigger(collider.IsTrigger()))
			return false;

		if (contactFilter.IsFilteringLayerMask(pOwner->GetLayer()))
			return false;

		return !contactFilter.IsFilteringDepth(0.0f);
	}

	/*----------------------------------------------------------------------------
	| --- CopyHits: 
	----------------------------------------------------------------------------*/
	int CopyHits(const std::vector<CaledonEngine::ColliderHit2D>& hits, std::span<CaledonEngine::ColliderHit2D> results)
	{
		const std::size_t count = std::min(hits.size(), results.size());
		std::copy_n(hits.begin(), count, results.begin());
		return static_cast<int>(count);
	}

	/*----------------------------------------------------------------------------
	| --- CollidersOf:
	----------------------------------------------------------------------------*/
	std::vector<CaledonEngine::Collider2D*> CollidersOf(const std::vector<CaledonEngine::ColliderHit2D>& hits)
	{
		std::vector<CaledonEngine::Collider2D*> colliders;
		colliders.reserve(hits.size());

		for (const CaledonEngine::ColliderHit2D& hit : hits)
		{
			colliders.push_back(hit.pCollider);
		}

		return colliders;
	}

	/*----------------------------------------------------------------------------
	| --- MakeCircleView:
	----------------------------------------------------------------------------*/
	CaledonEngine::ShapeView2D MakeCircleView(const CaledonEngine::Vector2f& center, float radius)
	{
		CaledonEngine::ShapeView2D view;
		view.type = CaledonEngine::PhysicsShapeType2D::Circle;
		view.vertices = std::span<const CaledonEngine::Vector2f>(&center, 1);
		view.radius = radius;
		return view;
	}
}

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
	std::vector<ColliderHit2D> hits;
	QueryRay(origin, direction, distance, radius, contactFilter, hits);
	return hits;
}

/*---------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return CopyHits(CircleCast(origin, radius, direction, contactFilter, distance), results);
}

/*--------------------------------------------------------------------------------------------------------------------
| --- CircleCastAll: Returns all of the unique Colliders intersected by the casted Circle in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::CircleCastAll(const Vector2f& origin, float radius, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return CollidersOf(CircleCast(origin, radius, direction, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers), distance));
}

/*---------------------------------------------------------------------------------------------------------
| --- Raycast: Returns all intersections of a casted Ray against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	std::vector<ColliderHit2D> hits;
	QueryRay(origin, direction, distance, 0.0f, contactFilter, hits);
	return hits;
}

/*---------------------------------------------------------------------------------------------------------
| --- Raycast: Returns all intersections of a casted Ray against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return CopyHits(Raycast(origin, direction, contactFilter, distance), results);
}

/*--------------------------------------------------------------------------------------------------------------
| --- RaycastAll: Returns all of the unique Colliders intersected by the casted Ray in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::RaycastAll(const Vector2f& origin, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return CollidersOf(Raycast(origin, direction, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers), distance));
}

/*-------------------------------------------------------------------------------------------------------------------
| --- Linecast: Returns all intersections of a casted Line segment against the Colliders in this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter) const
{
	const Vector2f line = end - start;
	const float length = line.Magnitude();

	if (!(length > 0.0f))
		return std::vector<ColliderHit2D>();

	return Raycast(start, line / length, contactFilter, length);
}

/*-------------------------------------------------------------------------------------------------------------------
| --- Linecast: Returns all intersections of a casted Line segment against the Colliders in this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return CopyHits(Linecast(start, end, contactFilter), results);
}

/*------------------------------------------------------------------------------------------------------------------------
| --- LinecastAll: Returns all of the unique Colliders intersected by the casted Line segment in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::LinecastAll(const Vector2f& start, const Vector2f& end, int layerMask, float minDepth, float maxDepth) const
{
	return CollidersOf(Linecast(start, end, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers)));
}

/*---------------------------------------------------------------------------------------------------------------
| --- GetRayIntersection: Returns all intersections of a Ray2D against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::GetRayIntersection(const Ray2D& ray, float distance, int layerMask) const
{
	return Raycast(ray.m_origin, ray.m_direction, ContactFilter2D::CreateLegacyFilter(layerMask, -Math::Infinity, Math::Infinity, m_pSettings->queriesHitTriggers), distance);
}

/*---------------------------------------------------------------------------------------------------------------
| --- GetRayIntersection: Returns all intersections of a Ray2D against the Colliders in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::GetRayIntersection(const Ray2D& ray, float distance, int layerMask, std::span<ColliderHit2D> results) const
{
	return CopyHits(GetRayIntersection(ray, distance, layerMask), results);
}

/*------------------------------------------------------------------------------------------------------------------------
| --- GetRayIntersectionAll: Returns all of the unique Colliders interesected against a Ray2D in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::PhysicsScene2D::GetRayIntersectionAll(const Ray2D& ray, float distance, int layerMask) const
{
	return GetRayIntersection(ray, distance, layerMask);
}

/*--------------------------------------------------------------------------------------------------------------------
| --- OverlapArea: Returns all intersections of an overlapping Area against the Colliders in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapArea(pointA, pointB, contactFilter, results);
	return results;
}

/*--------------------------------------------------------------------------------------------------------------------
| --- OverlapArea: Returns all intersections of an overlapping Area against the Colliders in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	const Vector2f center = (pointA + pointB) * 0.5f;
	const Vector2f size{ std::fabs(pointB.x - pointA.x), std::fabs(pointB.y - pointA.y) };
	return OverlapBox(center, size, 0.0f, contactFilter, results);
}

/*------------------------------------------------------------------------------------------------------------------------
| --- OverlapAreaAll: Returns all of the unique Colliders intersected by the overlapping Area in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapAreaAll(const Vector2f& pointA, const Vector2f& pointB, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapArea(pointA, pointB, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
}

/*------------------------------------------------------------------------------------------------------------------
| --- OverlapBox: Returns all intersections of an overlapping Box against the Colliders in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapBox(point, size, angle, contactFilter, results);
	return results;
}

/*------------------------------------------------------------------------------------------------------------------
| --- OverlapBox: Returns all intersections of an overlapping Box against the Colliders in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	PhysicsShapeGroup2D boxGroup;
	if (boxGroup.AddBox(point, size, angle) < 0)
	{
		results.clear();
		return 0;
	}

	const ShapeView2D queryShape = ShapeQueries2D::MakeView(boxGroup, 0);
	return QueryOverlaps(std::span<const ShapeView2D>(&queryShape, 1), contactFilter, nullptr, results);
}

/*----------------------------------------------------------------------------------------------------------------------
| --- OverlapBoxAll: Returns all of the unique Colliders intersected by the overlapping Box in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapBoxAll(const Vector2f& point, const Vector2f& size, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapBox(point, size, angle, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
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
	std::vector<Collider2D*> results;
	OverlapCircle(point, radius, contactFilter, results);
	return results;
}

/*------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Colliders in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	if (!(radius >= 0.0f))
	{
		results.clear();
		return 0;
	}

	const ShapeView2D queryShape = MakeCircleView(point, radius);
	return QueryOverlaps(std::span<const ShapeView2D>(&queryShape, 1), contactFilter, nullptr, results);
}

/*----------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircleAll: Returns all of the unique Colliders intersected by the overlapping Circle in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapCircleAll(const Vector2f& point, float radius, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapCircle(point, radius, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
}

/*----------------------------------------------------------------------------------------------------------------------------
| --- OverlapCollider: Returns all intersections of an overlapping Collider against the Colliders in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapCollider(pCollider, contactFilter, results);
	return results;
}

/*----------------------------------------------------------------------------------------------------------------------------
| --- OverlapCollider: Returns all intersections of an overlapping Collider against the Colliders in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	if (pCollider == nullptr)
	{
		results.clear();
		return 0;
	}

	PhysicsShapeGroup2D colliderShapes;
	pCollider->GetShapes(colliderShapes);

	std::vector<ShapeView2D> queryShapes;
	queryShapes.reserve(static_cast<std::size_t>(colliderShapes.GetShapeCount()));

	for (int i = 0; i < colliderShapes.GetShapeCount(); ++i)
	{
		queryShapes.push_back(ShapeQueries2D::MakeView(colliderShapes, i));
	}

	return QueryOverlaps(queryShapes, contactFilter, pCollider, results);
}

/*--------------------------------------------------------------------------------------------------------------------------------
| --- OverlapShapes: Returns all intersections of a group of world-space shapes against the Colliders in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapShapes(const PhysicsShapeGroup2D& shapes, const Collider2D* pSource, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	std::vector<ShapeView2D> queryShapes;
	queryShapes.reserve(static_cast<std::size_t>(shapes.GetShapeCount()));

	for (int i = 0; i < shapes.GetShapeCount(); ++i)
	{
		queryShapes.push_back(ShapeQueries2D::MakeView(shapes, i));
	}

	return QueryOverlaps(queryShapes, contactFilter, pSource, results);
}

/*----------------------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Returns all intersections of an overlapping Point against the Colliders in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapPoint(point, contactFilter, results);
	return results;
}

/*----------------------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Returns all intersections of an overlapping Point against the Colliders in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	const ShapeView2D queryShape = MakeCircleView(point, 0.0f);
	return QueryOverlaps(std::span<const ShapeView2D>(&queryShape, 1), contactFilter, nullptr, results);
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- OverlapPointAll: Returns all of the unique Colliders intersected by the overlapping Point in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::PhysicsScene2D::OverlapPointAll(const Vector2f& point, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapPoint(point, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- ClosestPoint:
--------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::PhysicsScene2D::ClosestPoint(const Vector2f& position, const Collider2D* pCollider) const
{
	if (pCollider == nullptr)
		return position;

	PhysicsShapeGroup2D shapes;
	pCollider->GetShapes(shapes);

	bool found = false;
	float bestSqrDistance = 0.0f;
	Vector2f best = position;
	for (int i = 0; i < shapes.GetShapeCount(); ++i)
	{
		const ShapeView2D view = ShapeQueries2D::MakeView(shapes, i);
		if (ShapeQueries2D::OverlapPoint(view, position))
			return position;

		Vector2f candidate;
		if (!ShapeQueries2D::ClosestPointOnPerimeter(view, position, candidate))
			continue;

		const float sqrDistance = Vector2f::SqrDistance(position, candidate);
		if (!found || sqrDistance < bestSqrDistance)
		{
			best = candidate;
			bestSqrDistance = sqrDistance;
			found = true;
		}
	}

	return best;
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



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------
| --- QueryOverlaps:
-------------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::QueryOverlaps(std::span<const ShapeView2D> queryShapes, const ContactFilter2D& contactFilter, const Collider2D* pSource, std::vector<Collider2D*>& results) const
{
	results.clear();

	PhysicsShapeGroup2D candidateShapes;
	for (Collider2D* pCandidate : m_registeredColliders)
	{
		if (pCandidate == pSource)
			continue;

		if (pSource != nullptr && !pSource->CanContact(pCandidate))
			continue;

		if (!PassesFilter(*pCandidate, contactFilter))
			continue;

		candidateShapes.Clear();
		pCandidate->GetShapes(candidateShapes);

		bool overlaps = false;
		for (int i = 0; i < candidateShapes.GetShapeCount() && !overlaps; ++i)
		{
			const ShapeView2D candidateView = ShapeQueries2D::MakeView(candidateShapes, i);
			for (const ShapeView2D& queryShape : queryShapes)
			{
				if (ShapeQueries2D::Overlap(queryShape, candidateView))
				{
					overlaps = true;
					break;
				}
			}
		}

		if (overlaps)
		{
			results.push_back(pCandidate);
		}
	}

	return static_cast<int>(results.size());
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- QueryRay: 
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsScene2D::QueryRay(const Vector2f& origin, const Vector2f& direction, float distance, float castRadius, const ContactFilter2D& contactFilter, std::vector<ColliderHit2D>& hits) const
{
	hits.clear();

	const Vector2f rayDirection = direction.Normalized();
	if (!(rayDirection.SqrMagnitude() > 0.0f) || !(distance >= 0.0f) || !(castRadius >= 0.0f))
		return;

	PhysicsShapeGroup2D candidateShapes;
	for (Collider2D* pCandidate : m_registeredColliders)
	{
		if (!PassesFilter(*pCandidate, contactFilter))
			continue;

		candidateShapes.Clear();
		pCandidate->GetShapes(candidateShapes);

		bool found = false;
		ShapeRayHit2D nearest;
		Vector2f nearestCentroid;
		for (int i = 0; i < candidateShapes.GetShapeCount(); ++i)
		{
			const ShapeView2D shapeView = ShapeQueries2D::MakeView(candidateShapes, i);
			ShapeView2D grownView = shapeView;
			grownView.radius += castRadius;

			ShapeRayHit2D shapeHit;
			Vector2f centroid;
			if (!ShapeQueries2D::Raycast(grownView, origin, rayDirection, distance, m_pSettings->queriesStartInColliders, shapeHit))
				continue;

			if (castRadius > 0.0f)
			{
				centroid = shapeHit.point;
				if (shapeHit.distance > 0.0f)
				{
					shapeHit.point = shapeHit.point - shapeHit.normal * castRadius;
				}
				else if (!ShapeQueries2D::ClosestPointOnPerimeter(shapeView, origin, shapeHit.point))
					continue;
			}
			else
			{
				centroid = shapeHit.point;
			}

			if (contactFilter.IsFilteringNormalAngle(shapeHit.normal))
				continue;

			if (!found || shapeHit.distance < nearest.distance)
			{
				nearest = shapeHit;
				nearestCentroid = centroid;
				found = true;
			}
		}

		if (!found)
			continue;

		ColliderHit2D hit;
		hit.centroid = nearestCentroid;
		hit.pCollider = pCandidate;
		hit.distance = nearest.distance;
		hit.fraction = std::isfinite(distance) && distance > 0.0f ? nearest.distance / distance : 0.0f;
		hit.normal = nearest.normal;
		hit.point = nearest.point;
		hit.pRigidBody = pCandidate->GetAttachedRigidBody();
		hits.push_back(hit);
	}

	std::stable_sort(hits.begin(), hits.end(), [](const ColliderHit2D& a, const ColliderHit2D& b) { return a.distance < b.distance; });
}