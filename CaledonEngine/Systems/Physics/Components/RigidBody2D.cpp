/*------------------------------
| File: RigidBody2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "RigidBody2D.h"

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/PhysicsScene2D.h"
#include "Systems/Scene/Scene.h"
#include "Systems/Scene/Transform.h"


namespace
{
	using namespace CaledonEngine;

	constexpr float kMinMass = 0.0001f;
	constexpr float kMinInertia = 0.0001f;

	/*--------------------------------------------------------------------------------------------------------
	| --- ApplyLinearConstraints: Zeroes the components of a linear velocity that the constraints freeze --- |
	--------------------------------------------------------------------------------------------------------*/
	Vector2f ApplyLinearConstraints(const Vector2f& velocity, BodyConstraints2D constraints) noexcept
	{
		return Vector2f{ HasFlag(constraints, BodyConstraints2D::FreezePositionX) ? 0.0f : velocity.x,
						 HasFlag(constraints, BodyConstraints2D::FreezePositionY) ? 0.0f : velocity.y };
	}

	/*-----------------------------------------------------------------------------------------------------
	| --- ApplyPositionConstraints: Locks target position components to current coordinates if frozen --- |
	-----------------------------------------------------------------------------------------------------*/
	Vector2f ApplyPositionConstraints(const Vector2f& current, const Vector2f& target, BodyConstraints2D constraints) noexcept
	{
		return Vector2f{ HasFlag(constraints, BodyConstraints2D::FreezePositionX) ? current.x : target.x,
						 HasFlag(constraints, BodyConstraints2D::FreezePositionY) ? current.y : target.y };
	}
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------
| --- Constructor: Constructs this RigidBody2D with default values --- |
----------------------------------------------------------------------*/
CaledonEngine::RigidBody2D::RigidBody2D()
	: m_pPhysicsScene{ nullptr }
	, m_bodyType{ BodyType2D::Dynamic }
	, m_simulated{ true }
	, m_constraints{ BodyConstraints2D::None }
	, m_collisionDetectionMode{ CollisionDetectionMode2D::Discrete }
	, m_interpolation{ BodyInterpolation2D::None }
	, m_useFullKinematicContacts{ false }
	, m_sleepMode{ BodySleepMode2D::StartAwake }
	, m_isAwake{ true }
	, m_position{ 0.0f, 0.0f }
	, m_rotation{ 0.0f }
	, m_linearVelocity{ 0.0f, 0.0f }
	, m_angularVelocity{ 0.0f }
	, m_linearDamping{ 0.0f }
	, m_angularDamping{ 0.05f }
	, m_gravityScale{ 1.0f }
	, m_totalForce{ 0.0f, 0.0f }
	, m_totalTorque{ 0.0f }
	, m_useAutoMass{ false }
	, m_mass{ 1.0f }
	, m_centerOfMass{ 0.0f, 0.0f }
	, m_inertia{ 1.0f }
	, m_pAttachedColliders{ }
	, m_sharedMaterial{ }
	, m_includeLayers{ 0 }
	, m_excludeLayers{ 0 }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::RigidBody2D::~RigidBody2D()
{
	for (Collider2D* pCollider : m_pAttachedColliders)
	{
		if (pCollider != nullptr && pCollider->GetAttachedRigidBody() == this)
		{
			pCollider->SetAttachedRigidBody(nullptr);
		}
	}
	m_pAttachedColliders.clear();

	if (m_pPhysicsScene != nullptr)
	{
		m_pPhysicsScene->RemoveRigidBody2D(this);
	}
}

/*-------------------------------------------------------
| --- Initialize: Prepares this RigidBody2D for use --- |
-------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::Initialize()
{
	if (m_pPhysicsScene != nullptr)
		return true;

	const Scene* pScene = (m_pOwner != nullptr) ? m_pOwner->GetScene() : nullptr;
	m_pPhysicsScene = (pScene != nullptr) ? pScene->GetPhysicsScene2D() : nullptr;
	if (m_pPhysicsScene == nullptr)
		return false;

	if (!m_pPhysicsScene->AddRigidBody2D(this))
		return false;

	RefreshAttachedColliders();
	SyncFromTransform();
	m_isAwake = (m_sleepMode != BodySleepMode2D::StartAsleep);
	return true;
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
	if (m_bodyType == type)
		return;

	m_bodyType = type;

	if (type != BodyType2D::Dynamic)
	{
		ClearForces();
	}

	if (type == BodyType2D::Static)
	{
		m_linearVelocity = Vector2f{ 0.0f, 0.0f };
		m_angularVelocity = 0.0f;
	}
	else
	{
		WakeIfNeeded();
	}
}

/*---------------------------------------------------------------------------------
| --- SetFreezeRotation: Sets whether rotation is frozen for this RigidBody2D --- |
---------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetFreezeRotation(bool freeze) noexcept
{
	if (freeze)
	{
		m_constraints |= BodyConstraints2D::FreezeRotation;
		m_angularVelocity = 0.0f;
	}
	else
	{
		m_constraints &= ~BodyConstraints2D::FreezeRotation;
	}
}

/*------------------------------------------------------------------------------
| --- Sleep: Make this RigidBody2D 'sleep' (ignored for NeverSleep bodies) --- |
------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::Sleep()
{
	if (m_sleepMode == BodySleepMode2D::NeverSleep)
		return;

	m_isAwake = false;
	m_linearVelocity = Vector2f{ 0.0f, 0.0f };
	m_angularVelocity = 0.0f;
	ClearForces();
}

/*-------------------------------------------------------------------------------
| --- SetRotation: Sets the rotation of this RigidBody2D to the given angle --- |
-------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetRotation(float angle)
{
	m_rotation = angle;
	WakeIfNeeded();
}

/*--------------------------------------------------------------------
| --- MovePosition: Moves this RigidBody2D to the given position --- |
--------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MovePosition(const Vector2f& position)
{
	if (m_bodyType == BodyType2D::Static)
		return;

	m_position = ApplyPositionConstraints(m_position, position, m_constraints);
	WakeIfNeeded();
}

/*--------------------------------------------------------------------------------------------------
| --- MovePositionAndRotation: Moves this RigidBody2D to the given position and rotation angle --- |
--------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MovePositionAndRotation(const Vector2f& position, float angle)
{
	MovePosition(position);
	MoveRotation(angle);
}

/*--------------------------------------------------------------------------
| --- MoveRotation: Moves this RigidBody2D to the given rotation angle --- |
--------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MoveRotation(float angle)
{
	if (m_bodyType == BodyType2D::Static || GetFreezeRotation())
		return;

	m_rotation = angle;
	WakeIfNeeded();
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- Slide: Slide this RigidBody2D using the given velocity integrated over deltaTime using the specified SlideConfig --- |
--------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::SlideResults2D CaledonEngine::RigidBody2D::Slide(const Vector2f& velocity, float deltaTime, const SlideConfig2D& slideConfig)
{
	//...
}

/*----------------------------------------------------------------------------------------------------------------------------
| --- SyncFromTransform: Copies the owner's Transform pose into the body (called by the physics subsystem before a step) --- |
----------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SyncFromTransform()
{
	if (m_pOwner == nullptr)
		return;

	const Transform& transform = m_pOwner->GetTransform();
	m_position = transform.GetPosition();
	m_rotation = transform.GetRotation();
}

/*-----------------------------------------------------------------------------------------------------------------------------
| --- ApplyToTransform: Writes the body pose back to the owner's Transform (called by the physics subsystem after a step) --- |
-----------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::ApplyToTransform() const
{
	if (m_pOwner == nullptr)
		return;

	Transform& transform = m_pOwner->GetTransform();
	transform.SetPosition(m_position);
	transform.SetRotation(m_rotation);
}

/*-------------------------------------------------------------------------
| --- SetLinearVelocity: Sets the linear velocity of this RigidBody2D --- |
-------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetLinearVelocity(const Vector2f& velocity) noexcept
{
	if (m_bodyType == BodyType2D::Static)
		return;

	m_linearVelocity = ApplyLinearConstraints(velocity, m_constraints);
	if (m_linearVelocity != Vector2f::Zero())
	{
		WakeIfNeeded();
	}
}

/*---------------------------------------------------------------------------
| --- SetAngularVelocity: Sets the angular velocity of this RigidBody2D --- |
---------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetAngularVelocity(float velocity) noexcept
{
	if (m_bodyType == BodyType2D::Static)
		return;

	m_angularVelocity = GetFreezeRotation() ? 0.0f : velocity;
	if (m_angularVelocity != 0.0f)
	{
		WakeIfNeeded();
	}
}

/*-----------------------------------------------------
| --- AddForce: Apply a force to this RigidBody2D --- |
-----------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForce(const Vector2f& force, ForceMode2D mode)
{
	if (!CanReceiveForces())
		return;

	WakeIfNeeded();

	if (mode == ForceMode2D::Impulse)			// An impulse changes the velocity immediately
	{
		if (m_mass > 0.0f)
		{
			m_linearVelocity = ApplyLinearConstraints(m_linearVelocity + force / m_mass, m_constraints);
		}
	}
	else
	{
		m_totalForce += force;					// A force is accumulated and consumed by the next step
	}
}

/*------------------------------------------------------------------------------
| --- AddForceAtPosition: Apply a force at a given position in world-space --- |
------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceAtPosition(const Vector2f& force, const Vector2f& position, ForceMode2D mode)
{
	if (!CanReceiveForces())
		return;

	AddForce(force, mode);
	AddTorque(Vector2f::Cross(position - GetWorldCenterOfMass(), force), mode);
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- AddForceX: Adds a force to the X component of the linear velocity only leaving the Y component of the world-space untouched --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceX(float force, ForceMode2D mode)
{
	AddForce(Vector2f{ force, 0.0f }, mode);
}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- AddForceY: Adds a force to the Y component of the linear velocity only leaving the X component of the world-space untouched --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceY(float force, ForceMode2D mode)
{
	AddForce(Vector2f{ 0.0f, force }, mode);
}

/*---------------------------------------------------------------------------
| --- AddRelativeForce: Adds a force to the local-space linear velocity --- |
---------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForce(const Vector2f& relativeForce, ForceMode2D mode)
{
	AddForce(GetRelativePoint(relativeForce), mode);
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
| --- AddRelativeForceX: Adds a force to the X component of the linear velocity only leaving the Y component of the local-space untouched --- |
---------------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForceX(float force, ForceMode2D mode)
{
	AddRelativeForce(Vector2f{ force, 0.0f }, mode);
}

/*---------------------------------------------------------------------------------------------------------------------------------------------
| --- AddRelativeForceY: Adds a force to the Y component of the linear velocity only leaving the X component of the local-space untouched --- |
---------------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForceY(float force, ForceMode2D mode)
{
	AddRelativeForce(Vector2f{ 0.0f, force }, mode);
}

/*------------------------------------------------------------------------
| --- AddTorque: Apply a torque at this RigidBody2D's center of mass --- |
------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddTorque(float torque, ForceMode2D mode)
{
	if (!CanReceiveForces() || GetFreezeRotation())
		return;

	WakeIfNeeded();

	if (mode == ForceMode2D::Impulse)
	{
		if (m_inertia > 0.0f)
		{
			m_angularVelocity += (torque / m_inertia) * Math::Rad2Deg;
		}
	}
	else
	{
		m_totalTorque += torque;
	}
}

/*-----------------------------------------------------------------------------------------------
| --- SetUseAutoMass: Sets whether mass is automatically calculated from attached colliders --- |
-----------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetUseAutoMass(bool useAutoMass)
{
	m_useAutoMass = useAutoMass;
	if (m_useAutoMass)
	{
		UpdateMassData();
	}
}

/*----------------------------------------------------
| --- SetMass: Sets the mass of this RigidBody2D --- |
----------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetMass(float mass) noexcept
{
	m_mass = std::max(mass, kMinMass);
}

/*-------------------------------------------------------------------------------------------
| --- GetWorldCenterOfMass: Returns the world-space center of mass for this RigidBody2D --- |
-------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetWorldCenterOfMass() const noexcept
{
	return GetRelativePoint(m_centerOfMass);
}

/*------------------------------------------------------------------------------------------------------------------------------
| --- UpdateMassData: Computes the mass, center of mass, and inertia from the attached colliders (when useAutoMass = true) --- |
------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::UpdateMassData()
{
	if (!m_useAutoMass)
		return;

	//...
}

/*-----------------------------------------------------------------------------------------
| --- GetPoint: Returns a local space point given the point in rigidBody global space --- |
-----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetPoint(const Vector2f& point) const
{
	return (point - m_position).Rotate(-m_rotation);
}

/*--------------------------------------------------------------------------------------------
| --- GetVector: Returns a local space vector given the vector in rigidBody global space --- |
--------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetVector(const Vector2f& vector) const
{
	return vector.Rotate(-m_rotation);
}

/*---------------------------------------------------------------------------------------------------------------
| --- GetRelativePoint: Returns a global space point given the point relativePoint in rigidBody local space --- |
---------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativePoint(const Vector2f& relativePoint) const
{
	return relativePoint.Rotate(m_rotation) + m_position;
}

/*-------------------------------------------------------------------------------------------------------------------
| --- GetRelativeVector: Returns a global space vector given the vector relativeVector in rigidBody local space --- |
-------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativeVector(const Vector2f& relativeVector) const
{
	return relativeVector.Rotate(m_rotation);
}

/*-------------------------------------------------------------------------------------
| --- GetPointVelocity: The velocity of the RigidBody2D at a point in world space --- |
-------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetPointVelocity(const Vector2f& point) const
{
	const Vector2f offset = point - GetWorldCenterOfMass();
	return m_linearVelocity + offset.Perpendicular() * (m_angularVelocity * Math::Deg2Rad);
}

/*-------------------------------------------------------------------------------------------------
| --- GetRelativePointVelocity: The velocity of the RigidBody2D at a point in its local space --- |
-------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativePointVelocity(const Vector2f& relativePoint) const
{
	return GetPointVelocity(GetRelativePoint(relativePoint));
}

/*--------------------------------------------------------------------------------------------
| --- GetAttachedColliders: Returns all Collider2D that are attached to this RigidBody2D --- |
--------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetAttachedColliders(std::vector<Collider2D*>& results, bool findTriggers) const
{
	results.clear();
	for (Collider2D* pCollider : m_pAttachedColliders)
	{
		if (pCollider != nullptr && (findTriggers || !pCollider->IsTrigger()))
		{
			results.push_back(pCollider);
		}
	}
	return static_cast<int>(results.size());
}

/*------------------------------------------------------------------------------------------------
| --- RefreshAttachedColliders: Returns all Collider2D that are attached to this RigidBody2D --- |
------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::RefreshAttachedColliders()
{
	const std::vector<Collider2D*> previous = m_pAttachedColliders;
	for (Collider2D* pCollider : previous)
	{
		DetachCollider(pCollider);
	}

	if (m_pOwner == nullptr)
		return;

	for (Collider2D* pCollider : m_pOwner->GetComponents<Collider2D>())
	{
		AttachCollider(pCollider);
	}
}

/*---------------------------------------------------------------------------
| --- AttachCollider: Attaches a Collider2D; returns false if it failed --- |
---------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::AttachCollider(Collider2D* pCollider)
{
	if (pCollider == nullptr)
		return false;

	// Collider is already attached
	if (std::find(m_pAttachedColliders.begin(), m_pAttachedColliders.end(), pCollider) != m_pAttachedColliders.end())
		return false;

	// Collider belongs to another body; needs to be detached first
	if (pCollider->GetAttachedRigidBody() != nullptr && pCollider->GetAttachedRigidBody() != this)
		return false;

	m_pAttachedColliders.push_back(pCollider);
	pCollider->SetAttachedRigidBody(this);
	UpdateMassData();
	return true;
}

/*---------------------------------------------------------------------------
| --- DetachCollider: Detaches a Collider2D; returns false if it failed --- |
---------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::DetachCollider(Collider2D* pCollider)
{
	const auto it = std::find(m_pAttachedColliders.begin(), m_pAttachedColliders.end(), pCollider);

	if (pCollider == nullptr || it == m_pAttachedColliders.end())
		return false;

	m_pAttachedColliders.erase(it);

	if (pCollider->GetAttachedRigidBody() == this)
	{
		pCollider->SetAttachedRigidBody(nullptr);
	}

	UpdateMassData();
	return true;

}

/*-----------------------------------------------------------------------------
| --- SetSharedMaterial: Applies the material to every attached Collider2 --- |
-----------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetSharedMaterial(const PhysicsMaterial2D& material)
{
	m_sharedMaterial = material;

	for (Collider2D* pCollider : m_pAttachedColliders)
	{
		if (pCollider != nullptr)
		{
			pCollider->SetSharedMaterial(material);
		}
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------
| --- ClosestPoint: Returns a point on the perimeter of all enabled Colliders attached to this RigidBody that is closest to the specified position --- |
------------------------------------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::ClosestPoint(const Vector2f& position) const
{
	Vector2f best = position;
	float bestSqrDistance = Math::Infinity;
	for (const Collider2D* pCollider : m_pAttachedColliders)
	{
		if (pCollider == nullptr || !pCollider->IsActive())
			continue;

		const Vector2f point = pCollider->ClosestPoint(position);
		const float sqrDistance = Vector2f::SqrDistance(point, position);
		if (sqrDistance < bestSqrDistance)
		{
			bestSqrDistance = sqrDistance;
			best = point;
		}
	}
	return best;

}

/*------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Check if any of the RigidBody2D colliders overlap a given point in world-space --- |
------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::OverlapPoint(const Vector2f& point) const
{
	for (const Collider2D* pCollider : m_pAttachedColliders)
	{
		if (pCollider != nullptr && pCollider->IsActive() && pCollider->OverlapPoint(point))
			return true;
	}
	return false;

}

/*-------------------------------------------------------------------------------------------------------
| --- GetShapes: Returns all the PhysicsShape2D used by all Collider2D attached to this RigidBody2D --- |
-------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetShapes(PhysicsShapeGroup2D& physicsShapeGroup) const
{
	int count = 0;
	for (const Collider2D* pCollider : m_pAttachedColliders)
	{
		if (pCollider != nullptr && pCollider->IsActive())
		{
			count += pCollider->GetShapes(physicsShapeGroup);
		}
	}
	return count;

}

/*------------------------------------------------------------------------------------------------------------------------
| --- Distance: Calculates the minimum distance of this collider against all Collider2D attached to this RigidBody2D --- |
------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::RigidBody2D::Distance(const Collider2D* pCollider) const
{
	ColliderDistance2D best;
	if (pCollider == nullptr)
		return best;

	for (const Collider2D* pAttached : m_pAttachedColliders)
	{
		if (pAttached == nullptr || !pAttached->IsActive())
			continue;

		const ColliderDistance2D candidate = pAttached->Distance(pCollider);

		if (candidate.isValid && (!best.isValid || candidate.distance < best.distance))
		{
			best = candidate;
		}
	}
	return best;

}

/*------------------------------------------------------------------------------------------------------------------------
| --- IsTouching: Checks whether the collider is touching any of the collider(s) attached to this RigidBody2D or not --- |
------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsTouching(const Collider2D* pCollider) const
{
	for (const Collider2D* pAttached : m_pAttachedColliders)
	{
		if (pAttached != nullptr && pAttached->IsActive() && pAttached->IsTouching(pCollider))
			return true;
	}
	return false;

}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------
| --- IsTouchingLayers: Checks whether any of the collider(s) attached to this RigidBody2D are touching any colliders on the specified layerMask or not --- |
-----------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsTouchingLayers(LayerMask layerMask) const
{
	for (const Collider2D* pAttached : m_pAttachedColliders)
	{
		if (pAttached != nullptr && pAttached->IsActive() && pAttached->IsTouchingLayers(layerMask))
			return true;
	}
	return false;

}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| --- Cast: All the Collider2D shapes attached to this RigidBody2D are cast into the Scene starting at each Collider position ignoring the Colliders attached to the same RigidBody2D --- |
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::Cast(const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	size_t count = 0;
	for (const Collider2D* pCollider : m_pAttachedColliders)
	{
		if (count >= results.size())
			break;

		if (pCollider != nullptr && pCollider->IsActive())
		{
			// Each collider fills the space the previous ones left
			count += static_cast<size_t>(pCollider->Cast(direction, contactFilter, results.subspan(count), distance));
		}
	}
	return static_cast<int>(count);

}

/*----------------------------------------------------------------------------------------------------------
| --- Overlap: Returns a list of all Colliders that overlap all Colliders attached to this RigidBody2D --- |
----------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::Overlap(const Vector2f& position, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	results.clear();
	for (const Collider2D* pCollider : m_pAttachedColliders)
	{
		if (pCollider == nullptr || !pCollider->IsActive())
			continue;

		std::vector<Collider2D*> hits;
		pCollider->Overlap(position, angle, contactFilter, hits);

		for (Collider2D* pHit : hits)
		{
			const bool isOwnCollider = (pHit != nullptr && pHit->GetAttachedRigidBody() == this);

			if (pHit != nullptr && !isOwnCollider && std::find(results.begin(), results.end(), pHit) == results.end())
				results.push_back(pHit);
		}
	}
	return static_cast<int>(results.size());

}

/*---------------------------------------------------------------------------------------------------------
| --- GetContacts: Returns all contact points for all of the Collider(s) attached to this RigidBody2D --- |
---------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
	size_t count = 0;
	for (const Collider2D* pCollider : m_pAttachedColliders)
	{
		if (count >= contacts.size())
			break;

		if (pCollider != nullptr && pCollider->IsActive())
		{
			count += static_cast<size_t>(pCollider->GetContacts(contactFilter, contacts.subspan(count)));
		}
	}
	return static_cast<int>(count);

}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- GetContactColliders: Returns all colliders in contact with this RigidBody2D, with the results filtered by the contactFilter --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::RigidBody2D::GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const
{
	colliders.clear();
	for (const Collider2D* pCollider : m_pAttachedColliders)
	{
		if (pCollider == nullptr || !pCollider->IsActive())
			continue;

		std::vector<Collider2D*> touching;
		pCollider->GetContactColliders(contactFilter, touching);

		for (Collider2D* pTouching : touching)
		{
			if (pTouching != nullptr && std::find(colliders.begin(), colliders.end(), pTouching) == colliders.end())
			{
				colliders.push_back(pTouching);
			}
		}
	}
	return static_cast<int>(colliders.size());

}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*------------------------------------------------------------------------
| --- WakeIfNeeded: Forces and velocity changes wake a sleeping body --- |
------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::WakeIfNeeded() noexcept
{
	if (!m_isAwake && m_bodyType != BodyType2D::Static)
	{
		m_isAwake = true;
	}
}