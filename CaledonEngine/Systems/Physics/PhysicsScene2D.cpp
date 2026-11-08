/*------------------------------
| File: PhysicsScene2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "PhysicsScene2D.h"

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/Components/RigidBody2D.h"
#include "Systems/Physics/Materials/PhysicsMaterial2D.h"
#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"
#include "Systems/Physics/Shapes/ShapeCasts2D.h"
#include "Systems/Physics/Shapes/ShapeDistance2D.h"
#include "Systems/Physics/Types/ContactSolver2D.h"
#include "Systems/Scene/Component.h"
#include "Systems/Scene/GameObject.h"
#include "Systems/Scene/Transform.h"
#include "Utilities/Math/Constants.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <optional>
#include <set>
#include <unordered_map>

namespace
{
	using namespace CaledonEngine;

	/*-------------------------------------------------------------------------------------------------------
	| --- PassesFilter: Returns true if the Collider2D is active, owned, and passes the ContactFilter2D --- |
	-------------------------------------------------------------------------------------------------------*/
	bool PassesFilter(const Collider2D& collider, const ContactFilter2D& contactFilter)
	{
		const GameObject* pOwner = collider.GetOwner();
		if (!collider.IsActive() || pOwner == nullptr || !pOwner->IsActive())
			return false;

		if (contactFilter.IsFilteringTrigger(collider.IsTrigger()))
			return false;

		if (contactFilter.IsFilteringLayerMask(pOwner->GetLayer()))
			return false;

		return !contactFilter.IsFilteringDepth(0.0f);
	}

	/*------------------------------------------------------------------------------------------------------
	| --- CopyHits: Copies a subset of hit results into the target span up to the destination capacity --- |
	------------------------------------------------------------------------------------------------------*/
	int CopyHits(const std::vector<ColliderHit2D>& hits, std::span<ColliderHit2D> results)
	{
		const std::size_t count = std::min(hits.size(), results.size());
		std::copy_n(hits.begin(), count, results.begin());
		return static_cast<int>(count);
	}

	/*------------------------------------------------------------------------------------------------
	| --- CollidersOf: Extracts and returns a vector of raw Collider2D pointers from hit results --- |
	------------------------------------------------------------------------------------------------*/
	std::vector<Collider2D*> CollidersOf(const std::vector<ColliderHit2D>& hits)
	{
		std::vector<Collider2D*> colliders;
		colliders.reserve(hits.size());

		for (const ColliderHit2D& hit : hits)
		{
			colliders.push_back(hit.pCollider);
		}

		return colliders;
	}

	/*------------------------------------------------------------------------------------------------------------------------------
	| --- MakeCircleView: Builds a circle ShapeView2D around a center point (the view refers to center, which must outlive it) --- |
	------------------------------------------------------------------------------------------------------------------------------*/
	ShapeView2D MakeCircleView(const Vector2f& center, float radius)
	{
		ShapeView2D view;
		view.type = PhysicsShapeType2D::Circle;
		view.vertices = std::span<const Vector2f>(&center, 1);
		view.radius = radius;
		return view;
	}

	/*----------------------------------------------------------------------------------------------------------------------------
	| --- AddCapsuleShape: NOTE: Shouldn't this method be added to PhysicsShapeGroup2D?
	----------------------------------------------------------------------------------------------------------------------------*/
	bool AddCapsuleShape(PhysicsShapeGroup2D& group, const Vector2f& center, const Vector2f& size,
		CapsuleDirection2D capsuleDirection, float angle)
	{
		const bool vertical = (capsuleDirection == CapsuleDirection2D::Vertical);
		const float radius = 0.5f * (vertical ? size.x : size.y);
		const float halfLength = 0.5f * ((vertical ? size.y : size.x) - 2.0f * radius);
		if (!(radius > 0.0f) || !std::isfinite(radius) || !std::isfinite(halfLength))
			return false;

		if (!(halfLength > 0.0f))
			return group.AddCircle(center, radius) >= 0;

		const Vector2f axis = Vector2f{ vertical ? 0.0f : halfLength, vertical ? halfLength : 0.0f }.Rotate(angle);
		return group.AddCapsule(center - axis, center + axis, radius) >= 0;
	}

	/*----------------------------------------------------------------------------------------------------------------------------------------------------
	| --- PassesContactFilter: Returns whether the other Collider2D of a contact passes the ContactFilter2D (trigger, layer, depth and normal angle) --- |
	----------------------------------------------------------------------------------------------------------------------------------------------------*/
	bool PassesContactFilter(const Contact2D& contact, bool fromA, const ContactFilter2D& contactFilter)
	{
		const Collider2D* pOther = fromA ? contact.pColliderB : contact.pColliderA;
		if (contactFilter.IsFilteringTrigger(contact.isTrigger))
			return false;

		if (pOther->GetOwner() == nullptr || contactFilter.IsFilteringLayerMask(pOther->GetOwner()->GetLayer()))
			return false;

		if (contactFilter.IsFilteringDepth(0.0f))
			return false;

		if (!contact.isTrigger)
		{
			const Vector2f normal = fromA ? -contact.manifold.normal : contact.manifold.normal;
			if (contactFilter.IsFilteringNormalAngle(normal))
				return false;
		}
		return true;
	}

	/*----------------------------------------------------------------------------------------------------------------------------------
	| --- MakeContactPoint: Builds the ContactPoint2D of one manifold point, as seen from one of the two Collider2D of a Contact2D --- |
	----------------------------------------------------------------------------------------------------------------------------------*/
	ContactPoint2D MakeContactPoint(const Contact2D& contact, int pointIndex, bool fromA)
	{
		const ShapeManifoldPoint2D& manifoldPoint = contact.manifold.points[pointIndex];
		Collider2D* pThis = fromA ? contact.pColliderA : contact.pColliderB;
		Collider2D* pOther = fromA ? contact.pColliderB : contact.pColliderA;

		ContactPoint2D point;
		point.pCollider = pOther;
		point.pOtherCollider = pThis;
		point.pRigidBody = pOther->GetAttachedRigidBody();
		point.pOtherRigidBody = pThis->GetAttachedRigidBody();
		point.normal = fromA ? -contact.manifold.normal : contact.manifold.normal;
		point.point = manifoldPoint.point;
		point.relativeVelocity = fromA ? contact.velocityA[pointIndex] - contact.velocityB[pointIndex] : contact.velocityB[pointIndex] - contact.velocityA[pointIndex];
		point.bounciness = contact.bounciness;
		point.friction = contact.friction;
		point.separation = manifoldPoint.separation;
		point.normalImpulse = contact.normalImpulse[pointIndex];
		point.tangentImpulse = contact.tangentImpulse[pointIndex];
		point.enabled = true;
		return point;
	}

	enum class PairKind
	{
		None,				// These two colliders never touch
		Solid,				// They collide and push each other
		Trigger				// They only report that they overlap
	};

	/*----------------------------------------------------------------------------------------
	| --- IsMover: Returns true if the body is simulated and can move (it is not Static) --- |
	----------------------------------------------------------------------------------------*/
	bool IsMover(const RigidBody2D* pBody)
	{
		return pBody != nullptr && pBody->IsSimulated() && pBody->GetBodyType() != BodyType2D::Static;
	}

	/*-------------------------------------------------------------------------------------------------------------------------------------
	| --- ClassifyPair: Decides whether two Collider2D ignore each other, push each other (Solid) or only report an overlap (Trigger) --- |
	-------------------------------------------------------------------------------------------------------------------------------------*/
	PairKind ClassifyPair(const Collider2D& a, const Collider2D& b)
	{
		if (!a.CanContact(&b))
			return PairKind::None;

		const RigidBody2D* pBodyA = a.GetAttachedRigidBody();
		const RigidBody2D* pBodyB = b.GetAttachedRigidBody();

		if (a.IsTrigger() || b.IsTrigger())
			return (IsMover(pBodyA) || IsMover(pBodyB)) ? PairKind::Trigger : PairKind::None;

		const auto isDynamic = [](const RigidBody2D* pBody)
			{
				return IsMover(pBody) && pBody->GetBodyType() == BodyType2D::Dynamic;
			};

		if (isDynamic(pBodyA) || isDynamic(pBodyB))
			return PairKind::Solid;

		const auto isFullKinematic = [](const RigidBody2D* pBody)
			{
				return IsMover(pBody) && pBody->GetUseFullKinematicContacts();
			};

		return (isFullKinematic(pBodyA) || isFullKinematic(pBodyB)) ? PairKind::Solid : PairKind::None;
	}

	/*--------------------------------------------------------------------------------------------------------------------------------------------
	| --- PointVelocity: Returns the world-space velocity of a body at a point (its linear velocity plus the spin around its center of mass) --- |
	--------------------------------------------------------------------------------------------------------------------------------------------*/
	Vector2f PointVelocity(const RigidBody2D* pBody, const Vector2f& point)
	{
		if (pBody == nullptr)
			return Vector2f::Zero();

		const Vector2f arm = point - pBody->GetWorldCenterOfMass();
		const float omega = pBody->GetAngularVelocity() * Math::Deg2Rad;

		return pBody->GetLinearVelocity() + Vector2f{ -omega * arm.y, omega * arm.x };
	}

	/*--------------------------------------------------------------------------------------------------------------------------------------------------------------
	| --- ContactStepSolver: Turns the solid contacts of a step into solver bodies and constraints, solves them and writes the result back to the RigidBody2Ds --- |
	--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	class ContactStepSolver
	{
	private:
		enum class Role { Immovable, Kinematic, Dynamic };

		const Physics2DSettings& m_settings;
		ContactSolverSettings2D m_solverSettings;
		std::vector<SolverBody2D> m_bodies;
		std::vector<RigidBody2D*> m_rigidBodies;
		std::vector<Role> m_roles;
		std::vector<ContactConstraint2D> m_constraints;
		std::vector<std::size_t> m_contactIndices;
		std::unordered_map<const void*, int> m_bodyIndex;
		bool m_biasCleared = false;

		/*---------------------------------------------------------------------------------------------------------------------
		| --- ReadVelocities: Copies the velocities of a RigidBody2D into a solver body (the angular velocity in radians) --- |
		---------------------------------------------------------------------------------------------------------------------*/
		static void ReadVelocities(const RigidBody2D& body, SolverBody2D& solverBody)
		{
			solverBody.linearVelocity = body.GetLinearVelocity();
			solverBody.angularVelocity = body.GetAngularVelocity() * Math::Deg2Rad;
		}

		/*-------------------------------------------------------------------------------------------------------------------------------------------
		| --- GetBodyIndex: Returns the solver body index of the Collider2D's body, creating the solver body (pose, mass and role) on first use --- |
		-------------------------------------------------------------------------------------------------------------------------------------------*/
		int GetBodyIndex(Collider2D* pCollider)
		{
			RigidBody2D* pBody = pCollider->GetAttachedRigidBody();
			const void* key = pBody ? static_cast<const void*>(pBody) : static_cast<const void*>(pCollider);
			const auto found = m_bodyIndex.find(key);
			if (found != m_bodyIndex.end())
				return found->second;

			SolverBody2D solverBody;
			Role role = Role::Immovable;
			if (pBody != nullptr)
			{
				solverBody.position = pBody->GetWorldCenterOfMass();
				solverBody.angle = pBody->GetRotation() * Math::Deg2Rad;
				if (pBody->IsSimulated() && pBody->IsAwake() && pBody->GetBodyType() != BodyType2D::Static)
				{
					role = (pBody->GetBodyType() == BodyType2D::Dynamic) ? Role::Dynamic : Role::Kinematic;
					ReadVelocities(*pBody, solverBody);
				}
				if (role == Role::Dynamic)
				{
					solverBody.inverseMass = (pBody->GetMass() > 0.0f) ? 1.0f / pBody->GetMass() : 0.0f;
					solverBody.inverseInertia = (pBody->GetInertia() > 0.0f && !pBody->GetFreezeRotation()) ? 1.0f / pBody->GetInertia() : 0.0f;
					const BodyConstraints2D constraints = pBody->GetConstraints();
					solverBody.linearFactor.x = HasFlag(constraints, BodyConstraints2D::FreezePositionX) ? 0.0f : 1.0f;
					solverBody.linearFactor.y = HasFlag(constraints, BodyConstraints2D::FreezePositionY) ? 0.0f : 1.0f;
				}
			}
			else if (pCollider->GetOwner() != nullptr)
			{
				solverBody.position = pCollider->GetOwner()->GetTransform().GetPosition();
			}

			const int index = static_cast<int>(m_bodies.size());
			m_bodies.push_back(solverBody);
			m_rigidBodies.push_back(pBody);
			m_roles.push_back(role);
			m_bodyIndex.emplace(key, index);
			return index;
		}

	public:
		explicit ContactStepSolver(const Physics2DSettings& settings)
			: m_settings{ settings }
		{
			m_solverSettings.bounceThreshold = settings.bounceThreshold;
			m_solverSettings.baumgarte = settings.baumgarteScale;
			m_solverSettings.linearSlop = Physics2DConstants::kLinearSlop;
			m_solverSettings.maxLinearCorrection = settings.maxLinearCorrection;
		}

		/*-------------------------------------------------------------------------------------------------------------------------------------------------
		| --- Build: Creates a constraint for every solid Contact2D that involves a Dynamic body, warm started from the impulses of the previous step --- |
		-------------------------------------------------------------------------------------------------------------------------------------------------*/
		void Build(const std::vector<Contact2D>& contacts)
		{
			for (std::size_t i = 0; i < contacts.size(); ++i)
			{
				const Contact2D& contact = contacts[i];
				if (contact.isTrigger || contact.manifold.pointCount <= 0)
					continue;

				const int bodyA = GetBodyIndex(contact.pColliderA);
				const int bodyB = GetBodyIndex(contact.pColliderB);

				if (m_roles[static_cast<std::size_t>(bodyA)] != Role::Dynamic && m_roles[static_cast<std::size_t>(bodyB)] != Role::Dynamic)
					continue;

				ContactImpulses2D warm;
				for (int p = 0; p < contact.manifold.pointCount; ++p)
				{
					warm.normal[p] = contact.normalImpulse[p];
					warm.tangent[p] = contact.tangentImpulse[p];
				}

				m_constraints.push_back(ContactSolver2D::Build(contact.manifold, contact.friction, contact.bounciness, bodyA, bodyB, m_bodies, m_solverSettings, &warm));
				m_contactIndices.push_back(i);
			}
		}

		/*--------------------------------------------------------------------------------------------------
		| --- Handles: Returns true if the body takes part in the solve as a Kinematic or Dynamic body --- |
		--------------------------------------------------------------------------------------------------*/
		bool Handles(const RigidBody2D* pBody) const
		{
			const auto found = m_bodyIndex.find(pBody);
			return found != m_bodyIndex.end() && m_roles[static_cast<std::size_t>(found->second)] != Role::Immovable;
		}

		/*--------------------------------------------------------------------------------------------------------------------------------------------------------------
		| --- Solve: Warm starts and solves the velocities, moves the bodies over deltaTime, corrects their positions and writes the new state to the RigidBody2Ds --- |
		--------------------------------------------------------------------------------------------------------------------------------------------------------------*/
		void Solve(float deltaTime)
		{
			if (m_constraints.empty())
				return;

			for (std::size_t i = 0; i < m_bodies.size(); ++i)
			{
				if (m_roles[i] != Role::Immovable)
					ReadVelocities(*m_rigidBodies[i], m_bodies[i]);
			}

			ContactSolver2D::WarmStart(m_constraints, m_bodies);

			for (int iteration = 0; iteration < m_settings.velocityIterations; ++iteration)
			{
				ContactSolver2D::SolveVelocity(m_constraints, m_bodies);
			}

			if (!m_biasCleared)
			{
				for (auto& constraint : m_constraints)
				{
					for (int p = 0; p < constraint.pointCount; ++p)
					{
						constraint.points[p].velocityBias = 0.0f;
					}
				}
				m_biasCleared = true;
			}

			const float maxRotation = m_settings.maxRotationSpeed * Math::Deg2Rad;
			for (std::size_t i = 0; i < m_bodies.size(); ++i)
			{
				if (m_roles[i] == Role::Immovable)
					continue;

				SolverBody2D& body = m_bodies[i];
				if (m_roles[i] == Role::Dynamic)
				{
					if (m_settings.maxTranslationSpeed > 0.0f)
					{
						body.linearVelocity = Vector2f::ClampMagnitude(body.linearVelocity, m_settings.maxTranslationSpeed);
					}

					if (maxRotation > 0.0f)
					{
						body.angularVelocity = std::clamp(body.angularVelocity, -maxRotation, maxRotation);
					}
				}
				body.position += body.linearVelocity * deltaTime;
				body.angle += body.angularVelocity * deltaTime;
			}

			for (int iteration = 0; iteration < m_settings.positionIterations; ++iteration)
			{
				if (ContactSolver2D::SolvePosition(m_constraints, m_bodies, m_solverSettings))
					break;
			}

			for (std::size_t i = 0; i < m_bodies.size(); ++i)
			{
				if (m_roles[i] == Role::Immovable)
					continue;

				RigidBody2D& body = *m_rigidBodies[i];
				if (m_roles[i] == Role::Dynamic)
				{
					body.SetLinearVelocity(m_bodies[i].linearVelocity);
					body.SetAngularVelocity(m_bodies[i].angularVelocity * Math::Rad2Deg);
				}
				body.SetPoseFromCenterOfMass(m_bodies[i].position, m_bodies[i].angle * Math::Rad2Deg);
			}
		}

		/*-------------------------------------------------------------------------------------------------------------------------
		| --- StoreImpulses: Saves the accumulated impulses back into the Contact2D so the next step can warm start from them --- |
		-------------------------------------------------------------------------------------------------------------------------*/
		void StoreImpulses(std::vector<Contact2D>& contacts) const
		{
			for (std::size_t k = 0; k < m_constraints.size(); ++k)
			{
				Contact2D& contact = contacts[m_contactIndices[k]];
				const ContactImpulses2D impulses = ContactSolver2D::GetImpulses(m_constraints[k]);

				for (int p = 0; p < m_constraints[k].pointCount; ++p)
				{
					contact.normalImpulse[p] = impulses.normal[p];
					contact.tangentImpulse[p] = impulses.tangent[p];
				}
			}
		}
	};
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------
| --- Constructor: Constructs this PhysicsScene2D with default values --- |
-------------------------------------------------------------------------*/
PhysicsScene2D::PhysicsScene2D(const Physics2DSettings& settings)
	: m_registeredColliders{ }
	, m_registeredBodies{ }
	, m_pSettings{ &settings }
	, m_subStepCount{ 0 }
	, m_subStepLostTime{ 0.0f }
	, m_isValid{ true }
{
}

/*---------------------------------------------------------------
| --- AddCollider: Adds a Collider2D to this PhysicsScene2D --- |
---------------------------------------------------------------*/
bool PhysicsScene2D::AddCollider(Collider2D* pCollider)
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
bool PhysicsScene2D::RemoveCollider(Collider2D* pCollider)
{
	const auto it = std::find(m_registeredColliders.begin(), m_registeredColliders.end(), pCollider);
	if (it == m_registeredColliders.end())
		return false;

	m_registeredColliders.erase(it);

	for (const Contact2D& contact : m_contacts)
	{
		if (contact.isTrigger || (contact.pColliderA != pCollider && contact.pColliderB != pCollider))
			continue;

		Collider2D* pOther = (contact.pColliderA == pCollider) ? contact.pColliderB : contact.pColliderA;
		if (RigidBody2D* pBody = pOther->GetAttachedRigidBody())
		{
			pBody->WakeUp();
		}
	}

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
bool PhysicsScene2D::AddRigidBody2D(RigidBody2D* pRigidBody2D)
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
bool PhysicsScene2D::RemoveRigidBody2D(RigidBody2D* pRigidBody2D)
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
void PhysicsScene2D::WakeAllBodies() noexcept
{
	for (RigidBody2D* pBody : m_registeredBodies)
	{
		pBody->WakeUp();
	}
}

/*-----------------------------------------------------------------------------------------------
| --- GetIgnoreCollision: Checks whether collision between two Collider2D are to be ignored --- |
-----------------------------------------------------------------------------------------------*/
bool PhysicsScene2D::GetIgnoreCollision(const Collider2D* pCollider1, const Collider2D* pCollider2) const
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
void PhysicsScene2D::SetIgnoreCollision(Collider2D* pCollider1, Collider2D* pCollider2, bool ignore)
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

/*--------------------------------------------------------------------------------------------------------------------------------------
| --- GetContacts: Retrieves all of the ContactPoint2D with the given Collider2D, with the results filtered by the ContactFilter2D --- |
--------------------------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::GetContacts(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
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
		{
			contacts[count++] = MakeContactPoint(contact, i, fromA);
		}
	}
	return static_cast<int>(count);
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------
| --- GetContactColliders: Retrieves all of the unique Collider2D in contact with the given Collider2D, with the results filtered by the ContactFilter2D --- |
------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::GetContactColliders(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
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

/*-----------------------------------------------------------------------------------------------
| --- IsTouching: Returns whether the two Collider2D were in contact during the latest step --- |
-----------------------------------------------------------------------------------------------*/
bool PhysicsScene2D::IsTouching(const Collider2D* pCollider1, const Collider2D* pCollider2) const
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

/*---------------------------------------------------------------------------------------------------------------------------------------------------
| --- IsTouching: Returns whether the given Collider2D was touching any other Collider2D that passes the ContactFilter2D during the latest step --- |
---------------------------------------------------------------------------------------------------------------------------------------------------*/
bool PhysicsScene2D::IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> touching;
	return GetContactColliders(pCollider, contactFilter, touching) > 0;
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: --- |
-------------------------------------------------------------------------------------*/
bool PhysicsScene2D::IsTouching(const ContactFilter2D& contactFilter) const
{
	// TODO: Implement -> the contact filter is used to filter the results differently, such as by layer mask, Z depth, or normal angle
	//					  returns whether the Collider is touching any other Collider filtered by the ContactFilter2D or not
	return false;
}

/*--------------------------------------------------------------------------------------------------------------------
| --- IsTouchingLayers: Returns whether any Collider2D is touching another Collider2D on the specified LayerMask --- |
--------------------------------------------------------------------------------------------------------------------*/
bool PhysicsScene2D::IsTouchingLayers(LayerMask layerMask) const
{
	ContactFilter2D filter = ContactFilter2D::NoFilter();
	filter.SetLayerMask(layerMask);
	return IsTouching(filter);
}

/*----------------------------------------------------------------------------------------------------------
| --- BoxCast: Returns all intersections of a casted Box against the Collider2D in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------*/
std::vector<ColliderHit2D> PhysicsScene2D::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	PhysicsShapeGroup2D boxGroup;
	if (boxGroup.AddBox(origin, size, angle) < 0)
		return std::vector<ColliderHit2D>();

	const ShapeView2D movingShape = ShapeQueries2D::MakeView(boxGroup, 0);
	std::vector<ColliderHit2D> hits;
	QueryShapeCast(std::span<const ShapeView2D>(&movingShape, 1), origin, direction, distance, contactFilter, nullptr, false, hits);
	return hits;
}

/*---------------------------------------------------------------------------------------------------------
| --- BoxCast: Returns all intersections of a casted Box against the Collider2D in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return CopyHits(BoxCast(origin, size, angle, direction, contactFilter, distance), results);
}

/*--------------------------------------------------------------------------------------------------------------
| --- BoxCastAll: Returns all of the unique Collider2D intersected by the casted Box in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::BoxCastAll(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return CollidersOf(BoxCast(origin, size, angle, direction, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers), distance));
}

/*-----------------------------------------------------------------------------------------------------------------
| --- CapsuleCast: Returns all intersections of a casted Capsule against the Collider2D in this PhysicsScene2D --- |
-----------------------------------------------------------------------------------------------------------------*/
std::vector<ColliderHit2D> PhysicsScene2D::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	PhysicsShapeGroup2D capsuleGroup;
	if (!AddCapsuleShape(capsuleGroup, origin, size, capsuleDirection, angle))
		return std::vector<ColliderHit2D>();

	const ShapeView2D movingShape = ShapeQueries2D::MakeView(capsuleGroup, 0);
	std::vector<ColliderHit2D> hits;
	QueryShapeCast(std::span<const ShapeView2D>(&movingShape, 1), origin, direction, distance, contactFilter, nullptr, false, hits);
	return hits;
}

/*-----------------------------------------------------------------------------------------------------------------
| --- CapsuleCast: Returns all intersections of a casted Capsule against the Collider2D in this PhysicsScene2D --- |
-----------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return CopyHits(CapsuleCast(origin, size, capsuleDirection, angle, direction, contactFilter, distance), results);
}

/*----------------------------------------------------------------------------------------------------------------------
| --- CapsuleCastAll: Returns all of the unique Collider2D intersected by the casted Capsule in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::CapsuleCastAll(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return CollidersOf(CapsuleCast(origin, size, capsuleDirection, angle, direction, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers), distance));
}

/*---------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Collider2D in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
std::vector<ColliderHit2D> PhysicsScene2D::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	std::vector<ColliderHit2D> hits;
	QueryRay(origin, direction, distance, radius, contactFilter, hits);
	return hits;
}

/*---------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Collider2D in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return CopyHits(CircleCast(origin, radius, direction, contactFilter, distance), results);
}

/*--------------------------------------------------------------------------------------------------------------------
| --- CircleCastAll: Returns all of the unique Collider2D intersected by the casted Circle in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::CircleCastAll(const Vector2f& origin, float radius, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return CollidersOf(CircleCast(origin, radius, direction, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers), distance));
}

/*------------------------------------------------------------------------------------------------------------------------------
| --- CastShapes: Casts a group of shapes along a direction and returns the number of hits recorded in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::CastShapes(const PhysicsShapeGroup2D& shapes, const Vector2f& reference, const Vector2f& direction, float distance, const Collider2D* pSource, bool ignoreSiblingColliders, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	std::vector<ShapeView2D> movingShapes;
	movingShapes.reserve(static_cast<std::size_t>(shapes.GetShapeCount()));

	for (int i = 0; i < shapes.GetShapeCount(); ++i)
	{
		movingShapes.push_back(ShapeQueries2D::MakeView(shapes, i));
	}

	std::vector<ColliderHit2D> hits;
	QueryShapeCast(movingShapes, reference, direction, distance, contactFilter, pSource, ignoreSiblingColliders, hits);
	return CopyHits(hits, results);
}

/*---------------------------------------------------------------------------------------------------------
| --- Raycast: Returns all intersections of a casted Ray against the Collider2D in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
std::vector<ColliderHit2D> PhysicsScene2D::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	std::vector<ColliderHit2D> hits;
	QueryRay(origin, direction, distance, 0.0f, contactFilter, hits);
	return hits;
}

/*---------------------------------------------------------------------------------------------------------
| --- Raycast: Returns all intersections of a casted Ray against the Collider2D in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return CopyHits(Raycast(origin, direction, contactFilter, distance), results);
}

/*--------------------------------------------------------------------------------------------------------------
| --- RaycastAll: Returns all of the unique Collider2D intersected by the casted Ray in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::RaycastAll(const Vector2f& origin, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return CollidersOf(Raycast(origin, direction, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers), distance));
}

/*-------------------------------------------------------------------------------------------------------------------
| --- Linecast: Returns all intersections of a casted Line segment against the Collider2D in this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------------------------*/
std::vector<ColliderHit2D> PhysicsScene2D::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter) const
{
	const Vector2f line = end - start;
	const float length = line.Magnitude();

	if (!(length > 0.0f))
		return std::vector<ColliderHit2D>();

	return Raycast(start, line / length, contactFilter, length);
}

/*-------------------------------------------------------------------------------------------------------------------
| --- Linecast: Returns all intersections of a casted Line segment against the Collider2D in this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return CopyHits(Linecast(start, end, contactFilter), results);
}

/*------------------------------------------------------------------------------------------------------------------------
| --- LinecastAll: Returns all of the unique Collider2D intersected by the casted Line segment in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::LinecastAll(const Vector2f& start, const Vector2f& end, int layerMask, float minDepth, float maxDepth) const
{
	return CollidersOf(Linecast(start, end, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers)));
}

/*---------------------------------------------------------------------------------------------------------------
| --- GetRayIntersection: Returns all intersections of a Ray2D against the Collider2D in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
std::vector<ColliderHit2D> PhysicsScene2D::GetRayIntersection(const Ray2D& ray, float distance, int layerMask) const
{
	return Raycast(ray.m_origin, ray.m_direction, ContactFilter2D::CreateLegacyFilter(layerMask, -Math::Infinity, Math::Infinity, m_pSettings->queriesHitTriggers), distance);
}

/*---------------------------------------------------------------------------------------------------------------
| --- GetRayIntersection: Returns all intersections of a Ray2D against the Collider2D in this PhysicsScene2D --- |
---------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::GetRayIntersection(const Ray2D& ray, float distance, int layerMask, std::span<ColliderHit2D> results) const
{
	return CopyHits(GetRayIntersection(ray, distance, layerMask), results);
}

/*-----------------------------------------------------------------------------------------------------------------------
| --- GetRayIntersectionAll: Returns all of the unique Collider2D intersected against a Ray2D in this PhysicsScene2D --- |
-----------------------------------------------------------------------------------------------------------------------*/
std::vector<ColliderHit2D> PhysicsScene2D::GetRayIntersectionAll(const Ray2D& ray, float distance, int layerMask) const
{
	return GetRayIntersection(ray, distance, layerMask);
}

/*--------------------------------------------------------------------------------------------------------------------
| --- OverlapArea: Returns all intersections of an overlapping Area against the Collider2D in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapArea(pointA, pointB, contactFilter, results);
	return results;
}

/*--------------------------------------------------------------------------------------------------------------------
| --- OverlapArea: Returns all intersections of an overlapping Area against the Collider2D in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	const Vector2f center = (pointA + pointB) * 0.5f;
	const Vector2f size{ std::fabs(pointB.x - pointA.x), std::fabs(pointB.y - pointA.y) };
	return OverlapBox(center, size, 0.0f, contactFilter, results);
}

/*------------------------------------------------------------------------------------------------------------------------
| --- OverlapAreaAll: Returns all of the unique Collider2D intersected by the overlapping Area in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapAreaAll(const Vector2f& pointA, const Vector2f& pointB, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapArea(pointA, pointB, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
}

/*------------------------------------------------------------------------------------------------------------------
| --- OverlapBox: Returns all intersections of an overlapping Box against the Collider2D in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapBox(point, size, angle, contactFilter, results);
	return results;
}

/*------------------------------------------------------------------------------------------------------------------
| --- OverlapBox: Returns all intersections of an overlapping Box against the Collider2D in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
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
| --- OverlapBoxAll: Returns all of the unique Collider2D intersected by the overlapping Box in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapBoxAll(const Vector2f& point, const Vector2f& size, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapBox(point, size, angle, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Collider2D in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapCapsule(point, size, capsuleDirection, angle, contactFilter, results);
	return results;
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Collider2D in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	PhysicsShapeGroup2D capsuleGroup;
	if (!AddCapsuleShape(capsuleGroup, point, size, capsuleDirection, angle))
	{
		results.clear();
		return 0;
	}

	const ShapeView2D queryShape = ShapeQueries2D::MakeView(capsuleGroup, 0);
	return QueryOverlaps(std::span<const ShapeView2D>(&queryShape, 1), contactFilter, nullptr, results);
}

/*------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsuleAll: Returns all of the unique Collider2D intersected by the overlapping Capsule in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapCapsuleAll(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapCapsule(point, size, capsuleDirection, angle, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
}

/*------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Collider2D in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapCircle(point, radius, contactFilter, results);
	return results;
}

/*------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Collider2D in this PhysicsScene2D --- |
------------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
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
| --- OverlapCircleAll: Returns all of the unique Collider2D intersected by the overlapping Circle in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapCircleAll(const Vector2f& point, float radius, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapCircle(point, radius, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
}

/*-------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCollider: Returns all intersections of an overlapping Collider2D against the Collider2D in this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapCollider(pCollider, contactFilter, results);
	return results;
}

/*-------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCollider: Returns all intersections of an overlapping Collider2D against the Collider2D in this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
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
| --- OverlapShapes: Returns all intersections of a group of world-space shapes against the Collider2D in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::OverlapShapes(const PhysicsShapeGroup2D& shapes, const Collider2D* pSource, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
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
| --- OverlapPoint: Returns all intersections of an overlapping Point against the Collider2D in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter) const
{
	std::vector<Collider2D*> results;
	OverlapPoint(point, contactFilter, results);
	return results;
}

/*----------------------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Returns all intersections of an overlapping Point against the Collider2D in this PhysicsScene2D --- |
----------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	const ShapeView2D queryShape = MakeCircleView(point, 0.0f);
	return QueryOverlaps(std::span<const ShapeView2D>(&queryShape, 1), contactFilter, nullptr, results);
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- OverlapPointAll: Returns all of the unique Collider2D intersected by the overlapping Point in this PhysicsScene2D --- |
--------------------------------------------------------------------------------------------------------------------------*/
std::vector<Collider2D*> PhysicsScene2D::OverlapPointAll(const Vector2f& point, int layerMask, float minDepth, float maxDepth) const
{
	return OverlapPoint(point, ContactFilter2D::CreateLegacyFilter(layerMask, minDepth, maxDepth, m_pSettings->queriesHitTriggers));
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- ClosestPoint: Returns a point on the perimeter of the given Collider2D that is closest to the specified position --- |
--------------------------------------------------------------------------------------------------------------------------*/
Vector2f PhysicsScene2D::ClosestPoint(const Vector2f& position, const Collider2D* pCollider) const
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

/*--------------------------------------------------------------------------
| --- Distance: Calculates the minimum distance between two Collider2D --- |
--------------------------------------------------------------------------*/
ColliderDistance2D PhysicsScene2D::Distance(const Collider2D* pColliderA, const Collider2D* pColliderB) const
{
	if (pColliderA == nullptr || pColliderB == nullptr || pColliderA == pColliderB)
		return ColliderDistance2D{};

	const auto isEnabled = [](const Collider2D* pCollider)
		{
			const GameObject* pOwner = pCollider->GetOwner();
			return pCollider->IsActive() && pOwner != nullptr && pOwner->IsActive();
		};

	if (!isEnabled(pColliderA) || !isEnabled(pColliderB))
		return ColliderDistance2D{};

	PhysicsShapeGroup2D shapesA;
	PhysicsShapeGroup2D shapesB;
	pColliderA->GetShapes(shapesA);
	pColliderB->GetShapes(shapesB);

	ColliderDistance2D best;
	for (int a = 0; a < shapesA.GetShapeCount(); ++a)
	{
		const ShapeView2D viewA = ShapeQueries2D::MakeView(shapesA, a);
		for (int b = 0; b < shapesB.GetShapeCount(); ++b)
		{
			ColliderDistance2D candidate;
			if (ShapeDistance2D::Compute(viewA, ShapeQueries2D::MakeView(shapesB, b), candidate) && (!best.isValid || candidate.distance < best.distance))
			{
				best = candidate;
			}
		}
	}
	return best;
}

/*-----------------------------------------------------------------------------------------------------
| --- SyncTransforms: Pushes the Transform poses of all registered bodies into the physics bodies --- |
-----------------------------------------------------------------------------------------------------*/
void PhysicsScene2D::SyncTransforms()
{
	for (RigidBody2D* pBody : m_registeredBodies)
	{
		pBody->SyncFromTransform();
	}
}

/*------------------------------------------------------------------------
| --- Simulate: Simulate physics associated with this PhysicsScene2D --- |
------------------------------------------------------------------------*/
bool PhysicsScene2D::Simulate(float deltaTime, int simulationLayers)
{
	m_subStepCount = 0;
	m_subStepLostTime = 0.0f;

	if (!m_isValid || !(deltaTime > 0.0f) || !std::isfinite(deltaTime))
		return false;

	SyncTransforms();
	UpdateContacts(simulationLayers);
	WakeTouchingBodies();

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

	ContactStepSolver solver(*m_pSettings);
	for (int step = 0; step < stepCount; ++step)
	{
		for (RigidBody2D* pBody : m_registeredBodies)
		{
			if (isSimulated(*pBody))
			{
				pBody->IntegrateVelocity(stepTime, *m_pSettings);
			}
		}

		if (step == 0)
		{
			solver.Build(m_contacts);
		}

		solver.Solve(stepTime);

		for (RigidBody2D* pBody : m_registeredBodies)
		{
			if (isSimulated(*pBody) && !solver.Handles(pBody))
			{
				pBody->IntegratePosition(stepTime);
			}
		}
	}
	solver.StoreImpulses(m_contacts);
	UpdateSleeping(deltaTime, simulationLayers);

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
/*-------------------------------------------------------------------------------------------------
| --- IsRegistered: Returns whether the given Collider2D is registered to this PhysicsScene2D --- |
-------------------------------------------------------------------------------------------------*/
bool PhysicsScene2D::IsRegistered(const Collider2D* pCollider) const
{
	return std::find(m_registeredColliders.begin(), m_registeredColliders.end(), pCollider) != m_registeredColliders.end();
}

/*-------------------------------------------------------------------------------------------------------------------------------------------
| --- UpdateContacts: Finds which shapes touch (brute force) and compares them with the previous step so contacts begin, persist or end --- |
-------------------------------------------------------------------------------------------------------------------------------------------*/
void PhysicsScene2D::UpdateContacts(int simulationLayers)
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

					if (!contact.isTrigger && old != previousIndex.end())
					{
						const Contact2D& before = previous[old->second];
						bool used[2] = { false, false };
						for (int p = 0; p < contact.manifold.pointCount; ++p)
						{
							int best = -1;
							float bestDistance = Physics2DConstants::kWarmStartMatchDistance * Physics2DConstants::kWarmStartMatchDistance;

							for (int q = 0; q < before.manifold.pointCount; ++q)
							{
								const Vector2f delta = contact.manifold.points[p].point - before.manifold.points[q].point;
								const float distance = delta.SqrMagnitude();

								if (!used[q] && distance <= bestDistance)
								{
									best = q;
									bestDistance = distance;
								}
							}
							if (best >= 0)
							{
								used[best] = true;
								contact.normalImpulse[p] = before.normalImpulse[best];
								contact.tangentImpulse[p] = before.tangentImpulse[best];
							}
						}
					}

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

/*----------------------------------------------------------------------------------------------------------------------------
| --- WakeTouchingBodies: Wakes the bodies that just lost a contact and any sleeping body that an awake body is touching --- |
----------------------------------------------------------------------------------------------------------------------------*/
void PhysicsScene2D::WakeTouchingBodies()
{
	const auto isMover = [](const RigidBody2D* pBody) { return pBody != nullptr && pBody->IsSimulated() && pBody->GetBodyType() != BodyType2D::Static; };

	for (const Contact2D& contact : m_endedContacts)
	{
		if (contact.isTrigger)
			continue;

		for (Collider2D* pCollider : { contact.pColliderA, contact.pColliderB })
		{
			if (!IsRegistered(pCollider))
				continue;

			if (RigidBody2D* pBody = pCollider->GetAttachedRigidBody(); isMover(pBody))
			{
				pBody->WakeUp();
			}
		}
	}

	bool changed = true;
	while (changed)
	{
		changed = false;
		for (const Contact2D& contact : m_contacts)
		{
			if (contact.isTrigger)
				continue;

			RigidBody2D* pBodyA = contact.pColliderA->GetAttachedRigidBody();
			RigidBody2D* pBodyB = contact.pColliderB->GetAttachedRigidBody();
			if (!isMover(pBodyA) || !isMover(pBodyB) || pBodyA->IsAwake() == pBodyB->IsAwake())
				continue;

			(pBodyA->IsAwake() ? pBodyB : pBodyA)->WakeUp();
			changed = true;
		}
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------
| --- UpdateSleeping: Groups touching bodies into islands and puts an island to sleep once all of its bodies have been still for timeToSleep --- |
------------------------------------------------------------------------------------------------------------------------------------------------*/
void PhysicsScene2D::UpdateSleeping(float deltaTime, int simulationLayers)
{
	std::vector<RigidBody2D*> nodes;
	std::unordered_map<const RigidBody2D*, int> nodeIndex;

	for (RigidBody2D* pBody : m_registeredBodies)
	{
		const GameObject* pOwner = pBody->GetOwner();
		if (pOwner == nullptr || !pBody->IsSimulated() || !pBody->IsAwake() || pBody->GetBodyType() == BodyType2D::Static)
			continue;

		const int layer = pOwner->GetLayer();
		if (layer < 0 || layer >= Physics2DConstants::kLayerCount || ((simulationLayers >> layer) & 1) == 0)
			continue;

		nodeIndex.emplace(pBody, static_cast<int>(nodes.size()));
		nodes.push_back(pBody);
	}

	std::vector<int> parent(nodes.size());
	for (std::size_t i = 0; i < parent.size(); ++i)
	{
		parent[i] = static_cast<int>(i);
	}

	const auto find = [&parent](int x)
		{
			while (parent[static_cast<std::size_t>(x)] != x)
			{
				parent[static_cast<std::size_t>(x)] = parent[static_cast<std::size_t>(parent[static_cast<std::size_t>(x)])];
				x = parent[static_cast<std::size_t>(x)];
			}
			return x;
		};

	for (const Contact2D& contact : m_contacts)
	{
		if (contact.isTrigger)
			continue;

		const auto a = nodeIndex.find(contact.pColliderA->GetAttachedRigidBody());
		const auto b = nodeIndex.find(contact.pColliderB->GetAttachedRigidBody());

		if (a == nodeIndex.end() || b == nodeIndex.end())
			continue;

		const int rootA = find(a->second);
		const int rootB = find(b->second);

		if (rootA != rootB)
		{
			parent[static_cast<std::size_t>(std::max(rootA, rootB))] = std::min(rootA, rootB);
		}
	}

	const float linearTolerance = m_pSettings->linearSleepTolerance;
	const float angularTolerance = m_pSettings->angularSleepTolerance;

	std::vector<float> islandSleepTime(nodes.size(), Math::MaxFloat);
	for (std::size_t i = 0; i < nodes.size(); ++i)
	{
		RigidBody2D* pBody = nodes[i];
		const Vector2f velocity = pBody->GetLinearVelocity();
		const bool still = pBody->GetSleepMode() != BodySleepMode2D::NeverSleep
			&& velocity.SqrMagnitude() <= linearTolerance * linearTolerance
			&& std::fabs(pBody->GetAngularVelocity()) <= angularTolerance;

		pBody->SetSleepTime(still ? pBody->GetSleepTime() + deltaTime : 0.0f);

		float& minimum = islandSleepTime[static_cast<std::size_t>(find(static_cast<int>(i)))];
		minimum = std::min(minimum, pBody->GetSleepTime());
	}

	for (std::size_t i = 0; i < nodes.size(); ++i)
	{
		if (islandSleepTime[static_cast<std::size_t>(find(static_cast<int>(i)))] >= m_pSettings->timeToSleep)
		{
			nodes[i]->Sleep();
		}
	}
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- DispatchContactEvents: Calls the collision and trigger callbacks (Enter, Stay and Exit) for the contacts of the latest step --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
void PhysicsScene2D::DispatchContactEvents()
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

		const auto isAwakeMover = [](const Collider2D* pCollider)
			{
				const RigidBody2D* pBody = pCollider->GetAttachedRigidBody();
				return pBody != nullptr && pBody->IsSimulated() && pBody->IsAwake() && pBody->GetBodyType() != BodyType2D::Static;
			};

		if (!allNew && !isAwakeMover(pairEvent.pA) && !isAwakeMover(pairEvent.pB))
		{
			i = end;
			continue;
		}

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
				const Vector2f velocity = pBody ? pBody->GetLinearVelocity() : Vector2f::Zero();
				const Vector2f otherVelocity = pOtherBody ? pOtherBody->GetLinearVelocity() : Vector2f::Zero();
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

/*-----------------------------------------------------------------------------------------------------------------------
| --- QueryRay: Casts a ray (thickened by castRadius) and records the nearest hit of each Collider2D, nearest first --- |
-----------------------------------------------------------------------------------------------------------------------*/
void PhysicsScene2D::QueryRay(const Vector2f& origin, const Vector2f& direction, float distance, float castRadius, const ContactFilter2D& contactFilter, std::vector<ColliderHit2D>& hits) const
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

/*--------------------------------------------------------------------------------------------------------------------------------------------------
| --- QueryOverlaps: Collects the colliders that overlap any of the query shapes, skipping the source and anything the ContactFilter2D rejects --- |
--------------------------------------------------------------------------------------------------------------------------------------------------*/
int PhysicsScene2D::QueryOverlaps(std::span<const ShapeView2D> queryShapes, const ContactFilter2D& contactFilter, const Collider2D* pSource, std::vector<Collider2D*>& results) const
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

/*----------------------------------------------------------------------------------------------------------------------------------
| --- QueryShapeCast: Sweeps the moving shapes along a direction and records the nearest hit of each Collider2D, nearest first --- |
----------------------------------------------------------------------------------------------------------------------------------*/
void PhysicsScene2D::QueryShapeCast(std::span<const ShapeView2D> movingShapes, const Vector2f& reference, const Vector2f& direction, float distance,
	const ContactFilter2D& contactFilter, const Collider2D* pSource, bool ignoreSiblingColliders, std::vector<ColliderHit2D>& hits) const
{
	hits.clear();

	const Vector2f castDirection = direction.Normalized();
	if (!(castDirection.SqrMagnitude() > 0.0f) || !(distance >= 0.0f))
		return;

	const RigidBody2D* pSourceBody = (pSource != nullptr) ? pSource->GetAttachedRigidBody() : nullptr;

	PhysicsShapeGroup2D candidateShapes;
	for (Collider2D* pCandidate : m_registeredColliders)
	{
		if (pCandidate == pSource)
			continue;

		if (ignoreSiblingColliders && pSourceBody != nullptr && pCandidate->GetAttachedRigidBody() == pSourceBody)
			continue;

		if (!PassesFilter(*pCandidate, contactFilter))
			continue;

		candidateShapes.Clear();
		pCandidate->GetShapes(candidateShapes);

		bool found = false;
		ShapeCastHit2D nearest;
		for (int i = 0; i < candidateShapes.GetShapeCount(); ++i)
		{
			const ShapeView2D targetView = ShapeQueries2D::MakeView(candidateShapes, i);
			for (const ShapeView2D& movingView : movingShapes)
			{
				ShapeCastHit2D shapeHit;
				if (!ShapeCasts2D::Cast(movingView, castDirection, distance, targetView, m_pSettings->queriesStartInColliders, shapeHit))
					continue;

				if (contactFilter.IsFilteringNormalAngle(shapeHit.normal))
					continue;

				if (!found || shapeHit.distance < nearest.distance)
				{
					nearest = shapeHit;
					found = true;
				}
			}
		}

		if (!found)
			continue;

		ColliderHit2D hit;
		hit.centroid = reference + castDirection * nearest.distance;
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