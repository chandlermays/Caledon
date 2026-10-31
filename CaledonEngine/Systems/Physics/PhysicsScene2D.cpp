/*------------------------------
| File: PhysicsScene2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "PhysicsScene2D.h"

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/Components/RigidBody2D.h"
#include "Systems/Physics/Materials/PhysicsMaterial2D.h"
#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"
#include "Systems/Scene/Component.h"
#include "Systems/Scene/GameObject.h"
#include "Utilities/Math/Constants.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <optional>
#include <set>
#include <unordered_map>

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

	enum class PairKind
	{
		None,				// These two colliders never touch
		Solid,				// They collide and push each other
		Trigger				// They only report that they overlap
	};

	/*------------------------------------------------------------------------------------------------------------------------------------
	| --- IsMover:
	------------------------------------------------------------------------------------------------------------------------------------*/
	bool IsMover(const CaledonEngine::RigidBody2D* pBody)
	{
		return pBody != nullptr && pBody->IsSimulated() && pBody->GetBodyType() != CaledonEngine::BodyType2D::Static;
	}

	/*------------------------------------------------------------------------------------------------------------------------------------
	| --- ClassifyPair: 
	------------------------------------------------------------------------------------------------------------------------------------*/
	PairKind ClassifyPair(const CaledonEngine::Collider2D& a, const CaledonEngine::Collider2D& b)
	{
		if (!a.CanContact(&b))
			return PairKind::None;

		const CaledonEngine::RigidBody2D* pBodyA = a.GetAttachedRigidBody();
		const CaledonEngine::RigidBody2D* pBodyB = b.GetAttachedRigidBody();

		if (a.IsTrigger() || b.IsTrigger())
			return (IsMover(pBodyA) || IsMover(pBodyB)) ? PairKind::Trigger : PairKind::None;

		const auto isDynamic = [](const CaledonEngine::RigidBody2D* pBody) { return IsMover(pBody) && pBody->GetBodyType() == CaledonEngine::BodyType2D::Dynamic; };
		if (isDynamic(pBodyA) || isDynamic(pBodyB))
			return PairKind::Solid;

		const auto isFullKinematic = [](const CaledonEngine::RigidBody2D* pBody)
			{
				return IsMover(pBody) && pBody->GetUseFullKinematicContacts();
			};

		return (isFullKinematic(pBodyA) || isFullKinematic(pBodyB)) ? PairKind::Solid : PairKind::None;
	}

	/*------------------------------------------------------------------------------------------------------------------------------------
	| --- PointVelocity: 
	------------------------------------------------------------------------------------------------------------------------------------*/
	CaledonEngine::Vector2f PointVelocity(const CaledonEngine::RigidBody2D* pBody, const CaledonEngine::Vector2f& point)
	{
		if (pBody == nullptr)
			return CaledonEngine::Vector2f{ 0.0f, 0.0f };

		const CaledonEngine::Vector2f arm = point - pBody->GetWorldCenterOfMass();
		const float omega = pBody->GetAngularVelocity() * CaledonEngine::Math::Deg2Rad;

		return pBody->GetLinearVelocity() + CaledonEngine::Vector2f{ -omega * arm.y, omega * arm.x };
	}

	/*------------------------------------------------------------------------------------------------------------------------------------
	| --- MakeContactPoint: 
	------------------------------------------------------------------------------------------------------------------------------------*/
	CaledonEngine::ContactPoint2D MakeContactPoint(const CaledonEngine::Contact2D& contact, int pointIndex, bool fromA)
	{
		const CaledonEngine::ShapeManifoldPoint2D& manifoldPoint = contact.manifold.points[pointIndex];
		CaledonEngine::Collider2D* pThis = fromA ? contact.pColliderA : contact.pColliderB;
		CaledonEngine::Collider2D* pOther = fromA ? contact.pColliderB : contact.pColliderA;

		CaledonEngine::ContactPoint2D point;
		point.pCollider = pOther;
		point.pOtherCollider = pThis;
		point.pRigidBody = pOther->GetAttachedRigidBody();
		point.pOtherRigidBody = pThis->GetAttachedRigidBody();
		point.normal = fromA ? contact.manifold.normal * -1.0f : contact.manifold.normal;
		point.point = manifoldPoint.point;
		point.relativeVelocity = fromA ? contact.velocityA[pointIndex] - contact.velocityB[pointIndex] : contact.velocityB[pointIndex] - contact.velocityA[pointIndex];
		point.bounciness = contact.bounciness;
		point.friction = contact.friction;
		point.separation = manifoldPoint.separation;
		point.normalImpulse = 0.0f;
		point.tangentImpulse = 0.0f;
		point.enabled = true;
		return point;
	}

	/*------------------------------------------------------------------------------------------------------------------------------------
	| --- PassesContactFilter: 
	------------------------------------------------------------------------------------------------------------------------------------*/
	bool PassesContactFilter(const CaledonEngine::Contact2D& contact, bool fromA, const CaledonEngine::ContactFilter2D& contactFilter)
	{
		const CaledonEngine::Collider2D* pOther = fromA ? contact.pColliderB : contact.pColliderA;
		if (contactFilter.IsFilteringTrigger(contact.isTrigger))
			return false;

		if (pOther->GetOwner() == nullptr || contactFilter.IsFilteringLayerMask(pOther->GetOwner()->GetLayer()))
			return false;

		if (contactFilter.IsFilteringDepth(0.0f))
			return false;

		if (!contact.isTrigger)
		{
			const CaledonEngine::Vector2f normal = fromA ? contact.manifold.normal * -1.0f : contact.manifold.normal;
			if (contactFilter.IsFilteringNormalAngle(normal))
				return false;
		}
		return true;
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

	m_registeredColliders.erase(it);

	const auto touchesRemoved = [pCollider](const Contact2D& contact) { return contact.pColliderA == pCollider || contact.pColliderB == pCollider; };
	m_contacts.erase(std::remove_if(m_contacts.begin(), m_contacts.end(), touchesRemoved), m_contacts.end());
	m_endedContacts.erase(std::remove_if(m_endedContacts.begin(), m_endedContacts.end(), touchesRemoved), m_endedContacts.end());

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

	m_registeredBodies.erase(it);
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

/*------------------------------------------------------------------------------------------------------------------------------------
| --- OverlapShapes: Returns all intersections of a group of world-space shapes against the Colliders in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------------------*/
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

	for (const Contact2D& contact : m_contacts)
	{
		if ((contact.pColliderA == pCollider1 && contact.pColliderB == pCollider2) || (contact.pColliderA == pCollider2 && contact.pColliderB == pCollider1))
			return true;
	}
	return false;
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- IsTouching:
--------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> touching;
	return GetContactColliders(pCollider, contactFilter, touching) > 0;
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

/*--------------------------------------------------------------------------------------------------------------------------
| --- GetContacts: Retrieves the contact points of the latest step for the Collider, filtered by the contactFilter --- |
| Only solid contacts have contact points. A contact with two manifold points gives two ContactPoint2D. The results fill the span in order |
--------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::GetContacts(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
	if (pCollider == nullptr)
		return 0;

	std::size_t count = 0;
	for (const Contact2D& contact : m_contacts)
	{
		if (contact.isTrigger)
			continue;

		const bool fromA = (contact.pColliderA == pCollider);
		if (!fromA && contact.pColliderB != pCollider)
			continue;

		if (!PassesContactFilter(contact, fromA, contactFilter))
			continue;

		for (int i = 0; i < contact.manifold.pointCount && count < contacts.size(); ++i)
			contacts[count++] = MakeContactPoint(contact, i, fromA);
	}
	return static_cast<int>(count);
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- GetContactColliders: 
--------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::PhysicsScene2D::GetContactColliders(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	results.clear();
	if (pCollider == nullptr)
		return 0;

	for (const Contact2D& contact : m_contacts)
	{
		const bool fromA = (contact.pColliderA == pCollider);
		if (!fromA && contact.pColliderB != pCollider)
			continue;

		if (!PassesContactFilter(contact, fromA, contactFilter))
			continue;

		Collider2D* pOther = fromA ? contact.pColliderB : contact.pColliderA;
		if (std::find(results.begin(), results.end(), pOther) == results.end())
		{
			results.push_back(pOther);
		}
	}
	return static_cast<int>(results.size());
}

/*-----------------------------------------------------------------------------------------------------
| --- SyncTransforms: Pushes the Transform poses of all registered bodies into the physics bodies --- |
-----------------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsScene2D::SyncTransforms()
{
	for (RigidBody2D* pBody : m_registeredBodies)
	{
		pBody->SyncFromTransform();
	}
}

/*------------------------------------------------------------------------
| --- Simulate: Simulate physics associated with this PhysicsScene2D --- |
------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::Simulate(float deltaTime, int simulationLayers)
{
	m_subStepCount = 0;
	m_subStepLostTime = 0.0f;

	if (!m_isValid || !(deltaTime > 0.0f) || !std::isfinite(deltaTime))
		return false;

	SyncTransforms();
	UpdateContacts(simulationLayers);

	int stepCount = 1;
	float stepTime = deltaTime;
	if (m_pSettings->useSubStepping && m_pSettings->minSubStepFPS > 0.0f)
	{
		const float maxStepTime = 1.0f / m_pSettings->minSubStepFPS;
		const int maxSteps = std::max(m_pSettings->maxSubStepCount, 1);
		const int neededSteps = static_cast<int>(std::ceil(deltaTime / maxStepTime));

		if (neededSteps > maxSteps)
		{
			stepCount = maxSteps;
			stepTime = maxStepTime;
			m_subStepLostTime = deltaTime - stepTime * static_cast<float>(stepCount);
		}
		else
		{
			stepCount = std::max(neededSteps, 1);
			stepTime = deltaTime / static_cast<float>(stepCount);
		}
	}
	m_subStepCount = stepCount;

	const auto isSimulated = [simulationLayers](const RigidBody2D& body)
		{
			const GameObject* pOwner = body.GetOwner();
			if (pOwner == nullptr)
				return false;

			const int layer = pOwner->GetLayer();
			return layer >= 0 && layer < Physics2DConstants::kLayerCount && ((simulationLayers >> layer) & 1) != 0;
		};

	for (int step = 0; step < stepCount; ++step)
	{
		for (RigidBody2D* pBody : m_registeredBodies)
		{
			if (isSimulated(*pBody))
			{
				pBody->Integrate(stepTime, *m_pSettings);
			}
		}
	}

	for (RigidBody2D* pBody : m_registeredBodies)
	{
		if (!isSimulated(*pBody))
			continue;

		pBody->ClearForces();
		if (pBody->IsSimulated() && pBody->GetBodyType() != BodyType2D::Static)
		{
			pBody->ApplyToTransform();
		}
	}

	DispatchContactEvents();
	return true;
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------
| --- IsRegistered: 
-------------------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsScene2D::IsRegistered(const Collider2D* pCollider) const
{
	return std::find(m_registeredColliders.begin(), m_registeredColliders.end(), pCollider) != m_registeredColliders.end();
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- DispatchContactEvents: 
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsScene2D::DispatchContactEvents()
{
	enum class Phase { Exit, Enter, Stay };
	struct PairEvent
	{
		Collider2D* pA;
		Collider2D* pB;
		Phase phase;
		bool isTrigger;
		std::vector<Contact2D> contacts;			// Only filled for Enter and Stay
	};

	std::vector<PairEvent> events;
	std::vector<PairEvent> enters;
	std::vector<PairEvent> stays;

	std::set<std::pair<const Collider2D*, const Collider2D*>> touching;
	for (std::size_t i = 0; i < m_contacts.size();)
	{
		std::size_t end = i;
		bool allNew = true;
		while (end < m_contacts.size() && m_contacts[end].pColliderA == m_contacts[i].pColliderA && m_contacts[end].pColliderB == m_contacts[i].pColliderB)
		{
			allNew = allNew && m_contacts[end].isNew;
			++end;
		}

		touching.emplace(m_contacts[i].pColliderA, m_contacts[i].pColliderB);

		PairEvent pairEvent{ m_contacts[i].pColliderA, m_contacts[i].pColliderB, allNew ? Phase::Enter : Phase::Stay, 
			m_contacts[i].isTrigger, std::vector<Contact2D>(m_contacts.begin() + static_cast<std::ptrdiff_t>(i), m_contacts.begin() + static_cast<std::ptrdiff_t>(end)) };

		(allNew ? enters : stays).push_back(std::move(pairEvent));
		i = end;
	}

	const auto isDisabled = [](const Collider2D* pCollider)
		{
			const GameObject* pOwner = pCollider->GetOwner();
			return !pCollider->IsActive() || pOwner == nullptr || !pOwner->IsActive();
		};

	for (std::size_t i = 0; i < m_endedContacts.size();)
	{
		const Contact2D& first = m_endedContacts[i];
		std::size_t end = i;
		while (end < m_endedContacts.size() && m_endedContacts[end].pColliderA == first.pColliderA && m_endedContacts[end].pColliderB == first.pColliderB)
			++end;

		const bool stillTouching = touching.count({ first.pColliderA, first.pColliderB }) != 0;
		const bool silenced = !m_pSettings->callbacksOnDisable && (isDisabled(first.pColliderA) || isDisabled(first.pColliderB));

		if (!stillTouching && !silenced)
		{
			events.push_back(PairEvent{ first.pColliderA, first.pColliderB, Phase::Exit, first.isTrigger, {} });
		}

		i = end;
	}

	events.insert(events.end(), std::make_move_iterator(enters.begin()), std::make_move_iterator(enters.end()));
	events.insert(events.end(), std::make_move_iterator(stays.begin()), std::make_move_iterator(stays.end()));

	for (const PairEvent& pairEvent : events)
	{
		for (int side = 0; side < 2; ++side)
		{
			Collider2D* pThis = (side == 0) ? pairEvent.pA : pairEvent.pB;
			Collider2D* pOther = (side == 0) ? pairEvent.pB : pairEvent.pA;

			if (!IsRegistered(pThis) || !IsRegistered(pOther))
				break;

			GameObject* pOwner = pThis->GetOwner();
			if (pOwner == nullptr || (pairEvent.phase != Phase::Exit && !pOwner->IsActive()))
				continue;

			Collision2D* pCollision = nullptr;
			std::optional<Collision2D> localCollision;

			if (!pairEvent.isTrigger)
			{
				RigidBody2D* pBody = pThis->GetAttachedRigidBody();
				RigidBody2D* pOtherBody = pOther->GetAttachedRigidBody();
				const Vector2f velocity = pBody ? pBody->GetLinearVelocity() : Vector2f{ 0.0f, 0.0f };
				const Vector2f otherVelocity = pOtherBody ? pOtherBody->GetLinearVelocity() : Vector2f{ 0.0f, 0.0f };
				const Vector2f relativeVelocity = velocity - otherVelocity;

				if (m_pSettings->reuseCollisionCallbacks)
				{
					if (m_pCollisionScratch)
					{
						m_pCollisionScratch->Reset(pOther, pThis, pOtherBody, pBody, relativeVelocity);
					}
					else
					{
						m_pCollisionScratch = std::make_unique<Collision2D>(pOther, pThis, pOtherBody, pBody, relativeVelocity);
					}
					pCollision = m_pCollisionScratch.get();
				}
				else
				{
					localCollision.emplace(pOther, pThis, pOtherBody, pBody, relativeVelocity);
					pCollision = &*localCollision;
				}

				for (const Contact2D& contact : pairEvent.contacts)
				{
					for (int point = 0; point < contact.manifold.pointCount; ++point)
					{
						pCollision->AddContact(MakeContactPoint(contact, point, contact.pColliderA == pThis));
					}
				}
			}

			for (Component* pComponent : pOwner->GetComponents<Component>())
			{
				if (!pComponent->IsActive())
					continue;

				switch (pairEvent.phase)
				{
				case Phase::Enter:	if (pairEvent.isTrigger) pComponent->OnTriggerEnter2D(pOther); else pComponent->OnCollisionEnter2D(*pCollision); break;
				case Phase::Stay:	if (pairEvent.isTrigger) pComponent->OnTriggerStay2D(pOther);  else pComponent->OnCollisionStay2D(*pCollision);  break;
				case Phase::Exit:	if (pairEvent.isTrigger) pComponent->OnTriggerExit2D(pOther);  else pComponent->OnCollisionExit2D(*pCollision);  break;
				}
			}
		}
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- UpdateContacts: 
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsScene2D::UpdateContacts(int simulationLayers)
{
	std::vector<Contact2D> previous = std::move(m_contacts);
	m_contacts.clear();
	m_endedContacts.clear();

	std::unordered_map<ContactKey2D, std::size_t, ContactKey2DHash> previousIndex;
	previousIndex.reserve(previous.size());

	for (std::size_t i = 0; i < previous.size(); ++i)
	{
		previousIndex.emplace(previous[i].GetKey(), i);
	}

	std::vector<char> persisted(previous.size(), 0);
	const std::size_t count = m_registeredColliders.size();
	std::vector<char> eligible(count, 0);
	std::vector<PhysicsShapeGroup2D> shapes(count);

	for (std::size_t i = 0; i < count; ++i)
	{
		const Collider2D* pCollider = m_registeredColliders[i];
		const GameObject* pOwner = pCollider->GetOwner();
		if (!pCollider->IsActive() || pOwner == nullptr || !pOwner->IsActive())
			continue;

		const int layer = pOwner->GetLayer();
		if (layer < 0 || layer >= Physics2DConstants::kLayerCount || ((simulationLayers >> layer) & 1) == 0)
			continue;

		eligible[i] = 1;
		pCollider->GetShapes(shapes[i]);
	}

	for (std::size_t i = 0; i < count; ++i)
	{
		if (!eligible[i])
			continue;

		for (std::size_t j = i + 1; j < count; ++j)
		{
			if (!eligible[j])
				continue;

			Collider2D* pA = m_registeredColliders[i];
			Collider2D* pB = m_registeredColliders[j];
			const PairKind kind = ClassifyPair(*pA, *pB);
			if (kind == PairKind::None)
				continue;

			const float friction = PhysicsMaterial2D::GetCombinedValues(pA->GetFriction(), pB->GetFriction(), pA->GetFrictionCombine(), pB->GetFrictionCombine());
			const float bounciness = PhysicsMaterial2D::GetCombinedValues(pA->GetBounciness(), pB->GetBounciness(), pA->GetBounceCombine(), pB->GetBounceCombine());

			for (int shapeA = 0; shapeA < shapes[i].GetShapeCount(); ++shapeA)
			{
				const ShapeView2D viewA = ShapeQueries2D::MakeView(shapes[i], shapeA);
				for (int shapeB = 0; shapeB < shapes[j].GetShapeCount(); ++shapeB)
				{
					const ShapeView2D viewB = ShapeQueries2D::MakeView(shapes[j], shapeB);

					Contact2D contact;
					contact.pColliderA = pA;
					contact.pColliderB = pB;
					contact.shapeIndexA = shapeA;
					contact.shapeIndexB = shapeB;
					contact.isTrigger = (kind == PairKind::Trigger);

					if (contact.isTrigger)
					{
						if (!ShapeQueries2D::Overlap(viewA, viewB))
							continue;
					}
					else
					{
						if (!ShapeManifold2DQueries::Collide(viewA, viewB, contact.manifold))
							continue;

						contact.friction = friction;
						contact.bounciness = bounciness;
						for (int p = 0; p < contact.manifold.pointCount; ++p)
						{
							contact.velocityA[p] = PointVelocity(pA->GetAttachedRigidBody(), contact.manifold.points[p].point);
							contact.velocityB[p] = PointVelocity(pB->GetAttachedRigidBody(), contact.manifold.points[p].point);
						}
					}

					const auto old = previousIndex.find(contact.GetKey());
					if (old != previousIndex.end())
					{
						persisted[old->second] = 1;
					}
					contact.isNew = (old == previousIndex.end());

					m_contacts.push_back(contact);
				}
			}
		}
	}

	for (std::size_t i = 0; i < previous.size(); ++i)
	{
		if (!persisted[i])
		{
			m_endedContacts.push_back(previous[i]);
		}
	}
}

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