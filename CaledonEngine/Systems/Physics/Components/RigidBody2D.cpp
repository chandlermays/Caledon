/*------------------------------
| File: RigidBody2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "RigidBody2D.h"

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/PhysicsScene2D.h"
#include "Systems/Scene/Scene.h"
#include "Systems/Scene/Transform.h"

/*---------------------------------------------------------------------------------------
| --- WakeIfNeeded:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::WakeIfNeeded() noexcept
{
	if (!m_isAwake && m_bodyType != BodyType2D::Static)
	{
		m_isAwake = true;
	}
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------
| --- Constructor: Constructs this RigidBody2D with default values --- |
----------------------------------------------------------------------*/
CaledonEngine::RigidBody2D::RigidBody2D()
	: m_bodyType{ BodyType2D::Dynamic }
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

	//...
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

	//...
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
		//...
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
	//...
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