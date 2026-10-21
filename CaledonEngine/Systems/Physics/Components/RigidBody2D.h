/*------------------------------
| File: RigidBody2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Scene/Component.h"
#include "Systems/Physics/Materials/PhysicsMaterial2D.h"
#include "Systems/Physics/Types/BodyConstraints2D.h"
#include "Systems/Physics/Types/BodyInterpolation2D.h"
#include "Systems/Physics/Types/BodySleepMode2D.h"
#include "Systems/Physics/Types/BodyType2D.h"
#include "Systems/Physics/Types/ColliderDistance2D.h"
#include "Systems/Physics/Types/CollisionDetectionMode2D.h"
#include "Systems/Physics/Types/ForceMode2D.h"
#include "Systems/Physics/Types/SlideResults2D.h"
#include "Systems/Physics/Types/SlideConfig2D.h"
#include "Systems/Physics/LayerMask.h"
#include "Utilities/Math/Vector2.h"

#include <span>
#include <vector>

// Provides physics movement and other dyanmics, and the ability to attach Collider2D to it

// This is a fundamental physics component that provides multiple simulation dynamics, such as RigidBody2D.position and RigidBody2D.rotation for pose control,
// and RigidBody2D.linearVelocity and RigidBody2D.angularVelocity for velocity control

// You can attach multiple Collider2D to a RigidBody2D to detect collisions and provide a collision response when you set RigidBody2D.bodyType to Dynamic

namespace CaledonEngine
{
	class Collider2D;
	struct ColliderHit2D;
	struct ContactFilter2D;
	struct ContactPoint2D;
	class PhysicsShapeGroup2D;

	class RigidBody2D : public Component
	{
	private:
		/* --- Body Type & Simulation State --- */
		BodyType2D m_bodyType;														// the physical behavior type of this RigidBody2D
		bool m_simulated;															// indicates whether the RigidBody2D should be simulated or not by the physics system
		BodyConstraints2D m_constraints;											// controls which degrees of freedom are allowed for the simulation of this RigidBody2D (freezeRotation is derived from this)
		CollisionDetectionMode2D m_collisionDetectionMode;							// the method used by the physics engine to check if two objects have collided
		BodyInterpolation2D m_interpolation;										// Physics interpolation used between updates
		bool m_useFullKinematicContacts;											// should kinematic/kinematic and kinematic/static collisions be allowed?
		BodySleepMode2D m_sleepMode;												// the sleep state that the RigidBody2D will initially be in
		bool m_isAwake;																// the current sleep state (true = awake)

		/* --- Transform --- */
		Vector2f m_position;														// the position of the RigidBody2D
		float m_rotation;															// the rotation of the RigidBody2D in degrees
		// Matrix4x4 m_localToWorldMatrix;											// the transformation matrix used to transform the RigidBody2D to world space

		/* --- Motion --- */
		Vector2f m_linearVelocity;													// the linear velocity of the RigidBody2D in world-units per second (the X and Y components are derived from this)
		float m_angularVelocity;													// angular velocity in degrees per second
		float m_linearDamping;														// the linear damping of the RigidBody2D linear velocity
		float m_angularDamping;														// the angular damping of this RigidBody2D angular velocity
		float m_gravityScale;														// the degree to which this object is affected by gravity
		Vector2f m_totalForce;														// the total amount of force that has been explicitly applied to this RigidBody2D since the last physics simulation step
		float m_totalTorque;														// the total amount of torque that has been explicitly applied to this RigidBody2D since the last physics simulation step

		/* --- Mass Data --- */
		bool m_useAutoMass;															// should the total RigidBody2D mass be automatically calculated from the Collider2D.density of attached colliders?
		float m_mass;																// mass of the RigidBody2D
		Vector2f m_centerOfMass;													// the center of mass of this RigidBody2D in local space
		float m_inertia;															// the RigidBody's resistance to changes in angular velocity (rotation)

		/* --- Colliders, Material & Layers --- */
		std::vector<Collider2D*> m_attachedColliders;								// the Collider2D attached to this RigidBody2D (non-owning; the count is the size)
		PhysicsMaterial2D m_sharedMaterial;											// the PhysicsMaterial2D that is applied to all Collider2D attached to this RigidBody2D
		LayerMask m_includeLayers;													// the additional Layers that all Collider2D attached to this RigidBody2D should include when deciding if a contact with another Collider2D should happen or not
		LayerMask m_excludeLayers;													// the additional Layers that all Collider2D attached to this RigidBody2D should exclude when deciding if a contact with another Collider2D should happen or not

		void WakeIfNeeded() noexcept;																								// Forces and velocity changes wake a sleeping body (not Static)
		bool CanReceiveForces() const noexcept { return m_bodyType == BodyType2D::Dynamic && m_simulated; }							// Only simulated Dynamic bodies respond to forces

	public:
		RigidBody2D();																// Constructor
		~RigidBody2D() override = default;											// Destructor
		RigidBody2D(const RigidBody2D&) = delete;									// Prevents copy-construction
		RigidBody2D& operator=(const RigidBody2D&) = delete;						// Prevents copy-assignment
		RigidBody2D(RigidBody2D&&) = delete;										// Prevents move-construction
		RigidBody2D& operator=(RigidBody2D&&) = delete;								// Prevents move-assignment

		bool Initialize() override;													// Attaches the Collider2D found on the owning GameObject and pulls the initial pose from its Transform
		const std::string& GetTypeName() const override;							// Returns the type name of this Component

		/*--------------------------------
		| --- Body Type & Simulation --- |
		--------------------------------*/
		BodyType2D GetBodyType() const noexcept										{ return m_bodyType; }
		void SetBodyType(BodyType2D type) noexcept;
		bool IsSimulated() const noexcept											{ return m_simulated; }
		void SetSimulated(bool simulated) noexcept									{ m_simulated = simulated; }
		BodyConstraints2D GetConstraints() const noexcept							{ return m_constraints; }
		void SetConstraints(BodyConstraints2D constraints) noexcept					{ m_constraints = constraints; }
		bool GetFreezeRotation() const noexcept										{ return HasFlag(m_constraints, BodyConstraints2D::FreezeRotation); }
		void SetFreezeRotation(bool freeze) noexcept;
		CollisionDetectionMode2D GetCollisionDetectionMode() const noexcept			{ return m_collisionDetectionMode; }
		void SetCollisionDetectionMode(CollisionDetectionMode2D mode) noexcept		{ m_collisionDetectionMode = mode; }
		BodyInterpolation2D GetInterpolation() const noexcept						{ return m_interpolation; }
		void SetInterpolation(BodyInterpolation2D interpolation) noexcept			{ m_interpolation = interpolation; }
		bool GetUseFullKinematicContacts() const noexcept							{ return m_useFullKinematicContacts; }
		void SetUseFullKinematicContacts(bool use) noexcept							{ m_useFullKinematicContacts = use; }

		/*---------------------------
		| --- Sleep & Wake-up --- |
		---------------------------*/
		BodySleepMode2D GetSleepMode() const noexcept								{ return m_sleepMode; }							// 
		void SetSleepMode(BodySleepMode2D mode) noexcept							{ m_sleepMode = mode; }							//
		bool IsAwake() const noexcept												{ return m_isAwake; }							// Is this RigidBody2D 'Awake'?
		bool IsSleeping() const noexcept											{ return !m_isAwake; }							// Is this RigidBody2D 'Sleeping'?
		void Sleep();																												// Make this RigidBody2D 'sleep' (ignored for NeverSleep bodies)
		void WakeUp() noexcept														{ m_isAwake = true; }							// Disables the 'sleeping' state of this RigidBody2D

		/*--------------------------
		| --- Pose & Movement --- |
		--------------------------*/
		const Vector2f& GetPosition() const noexcept								{ return m_position; }							// Gets the position of this RigidBody2D
		void SetPosition(const Vector2f& position) noexcept							{ m_position = position; WakeIfNeeded(); }		// Sets the position of this RigidBody2D to the given position
		float GetRotation() const noexcept											{ return m_rotation; }							// Gets the rotation of this RigidBody2D
		void SetRotation(float angle);																								// Sets the rotation of this RigidBody2D to the given angle
		void MovePosition(const Vector2f& position);																				// Moves this RigidBody2D to the given position
		void MovePositionAndRotation(const Vector2f& position, float angle);														// Moves this RigidBody2D to the given position and rotation angle
		void MoveRotation(float angle);																								// Moves this RigidBody2D to the given rotation angle
		SlideResults2D Slide(const Vector2f& velocity, float deltaTime, const SlideConfig2D& slideConfig);							// Slide this RigidBody2D using the given velocity integrated over deltaTime using the specified SlideConfig
		void SyncFromTransform();																									// Copies the owner's Transform pose into the body (called by the physics subsystem before a step)
		void ApplyToTransform() const;																								// Writes the body pose back to the owner's Transform (called by the physics subsystem after a step)

		/*-------------------------------------
		| --- Velocity, Damping & Gravity --- |
		-------------------------------------*/
		const Vector2f& GetLinearVelocity() const noexcept							{ return m_linearVelocity; }										// 
		void SetLinearVelocity(const Vector2f& velocity) noexcept;																						// 
		float GetLinearVelocityX() const noexcept									{ return m_linearVelocity.x; }										// 
		void SetLinearVelocityX(float x) noexcept									{ SetLinearVelocity(Vector2f{ x, m_linearVelocity.y }); }			// 
		float GetLinearVelocityY() const noexcept									{ return m_linearVelocity.y; }										// 
		void SetLinearVelocityY(float y) noexcept									{ SetLinearVelocity(Vector2f{ m_linearVelocity.x, y }); }			// 
		float GetAngularVelocity() const noexcept									{ return m_angularVelocity; }										// 
		void SetAngularVelocity(float velocity) noexcept;																								// 
		float GetLinearDamping() const noexcept										{ return m_linearDamping; }											// 
		void SetLinearDamping(float damping) noexcept								{ m_linearDamping = damping; }										// 
		float GetAngularDamping() const noexcept									{ return m_angularDamping; }										// 
		void SetAngularDamping(float damping) noexcept								{ m_angularDamping = damping; }										// 
		float GetGravityScale() const noexcept										{ return m_gravityScale; }											// 
		void SetGravityScale(float scale) noexcept									{ m_gravityScale = scale; }											// 

		/*-------------------------
		| --- Forces & Torque --- |
		-------------------------*/
		const Vector2f& GetTotalForce() const noexcept								{ return m_totalForce; }														//
		float GetTotalTorque() const noexcept										{ return m_totalTorque; }														//
		void ClearForces() noexcept													{ m_totalForce = Vector2f{ 0.0f, 0.0f }; m_totalTorque = 0.0f; }				// Called by the physics subsystem after each step
		
		void AddForce(const Vector2f& force, ForceMode2D mode = ForceMode2D::Force);												// Apply a force to this RigidBody2D
		void AddForceAtPosition(const Vector2f& force, const Vector2f& position, ForceMode2D mode = ForceMode2D::Force);			// Apply a force at a given position in world-space
		void AddForceX(float force, ForceMode2D mode = ForceMode2D::Force);															// Adds a force to the X component of the linear velocity only leaving the Y component of the world-space untouched
		void AddForceY(float force, ForceMode2D mode = ForceMode2D::Force);															// Adds a force to the Y component of the linear velocity only leaving the X component of the world-space untouched
		void AddRelativeForce(const Vector2f& relativeForce, ForceMode2D mode = ForceMode2D::Force);								// Adds a force to the local-space linear velocity
		void AddRelativeForceX(float force, ForceMode2D mode = ForceMode2D::Force);													// Adds a force to the X component of the linear velocity only leaving the Y component of the local-space untouched
		void AddRelativeForceY(float force, ForceMode2D mode = ForceMode2D::Force);													// Adds a force to the Y component of the linear velocity only leaving the X component of the local-space untouched
		void AddTorque(float torque, ForceMode2D mode = ForceMode2D::Force);														// Apply a torque at this RigidBody2D's center of mass

		/*-------------------
		| --- Mass Data --- |
		-------------------*/
		bool GetUseAutoMass() const noexcept										{ return m_useAutoMass; }						//
		void SetUseAutoMass(bool useAutoMass);																						//
		float GetMass() const noexcept												{ return m_mass; }								//
		void SetMass(float mass) noexcept;																							//
		const Vector2f& GetCenterOfMass() const noexcept							{ return m_centerOfMass; }						//
		void SetCenterOfMass(const Vector2f& centerOfMass) noexcept					{ m_centerOfMass = centerOfMass; }				//
		Vector2f GetWorldCenterOfMass() const noexcept;																				//
		float GetInertia() const noexcept											{ return m_inertia; }							//
		void SetInertia(float inertia) noexcept										{ m_inertia = inertia; }						//
		void UpdateMassData();																										// Computes the mass, center of mass, and inertia from the attached colliders (when useAutoMass = true)

		/*------------------------------------------------
		| --- 
		------------------------------------------------*/
		Vector2f GetPoint(const Vector2f& point) const;																				// Get a local space point given the point in rigidBody global space
		Vector2f GetVector(const Vector2f& vector) const;																			// Get a local space vector given the vector in rigidBody global space
		Vector2f GetRelativePoint(const Vector2f& relativePoint) const;																// Get a global space point given the point relativePoint in rigidBody local space
		Vector2f GetRelativeVector(const Vector2f& relativeVector) const;															// Get a global space vector given the vector relativeVector in rigidBody local space
		Vector2f GetPointVelocity(const Vector2f& point) const;																		// The velocity of the RigidBody2D at a point in world space
		Vector2f GetRelativePointVelocity(const Vector2f& relativePoint) const;														// The velocity of the RigidBody2D at a point in its local space

		/*--------------------------------------
		| --- Colliders, Material & Layers --- |
		--------------------------------------*/
		int GetAttachedColliderCount() const noexcept								{ return static_cast<int>(m_attachedColliders.size()); }
		int GetAttachedColliders(std::vector<Collider2D*>& results, bool findTriggers = true) const;												// Returns all Collider2D that are attached to this RigidBody2D (results cleared first)
		void RefreshAttachedColliders();																											// Re-scans the owning GameObject for Collider2D (children are not supported yet)
		bool AttachCollider(Collider2D* pCollider);																									// Attaches a Collider2D; returns false if it failed
		bool DetachCollider(Collider2D* pCollider);																									// Detaches a Collider2D; returns false if it failed
		const PhysicsMaterial2D& GetSharedMaterial() const noexcept					{ return m_sharedMaterial; }									//
		void SetSharedMaterial(const PhysicsMaterial2D& material);																					// Applies the material to every attached Collider2D
		const LayerMask& GetIncludeLayers() const noexcept							{ return m_includeLayers; }										//
		void SetIncludeLayers(LayerMask layers) noexcept							{ m_includeLayers = layers; }									//
		const LayerMask& GetExcludeLayers() const noexcept							{ return m_excludeLayers; }										//
		void SetExcludeLayers(LayerMask layers) noexcept							{ m_excludeLayers = layers; }									//

		/*----------------------------------------------
		| --- Queries over the attached Collider2D --- |
		----------------------------------------------*/
		Vector2f ClosestPoint(const Vector2f& position) const;																		// Returns a point on the perimeter of all enabled Colliders attached to this RigidBody that is closest to the specified position
		bool OverlapPoint(const Vector2f& point) const;																				// Check if any of the RigidBody2D colliders overlap a given point in world-space
		int GetShapes(PhysicsShapeGroup2D& physicsShapeGroup) const;																// Gets all the PhysicsShape2D used by all Collider2D attached to this RigidBody2D
		ColliderDistance2D Distance(const Collider2D* pCollider) const;																// Calculates the minimum distance of this collider against all Collider2D attached to this RigidBody2D
		bool IsTouching(const Collider2D* pCollider) const;																			// Checks whether the collider is touching any of the collider(s) attached to this RigidBody2D or not
		bool IsTouchingLayers(LayerMask layerMask = LayerMask{ -1 }) const;															// Checks whether any of the collider(s) attached to this RigidBody2D are touching any colliders on the specified layerMask or not
		int Cast(const Vector2f& direction, const ContactFilter2D& contactFilter,													// All the Collider2D shapes attached to this RigidBody2D are cast into the Scene starting at each Collider position ignoring the Colliders attached to the same RigidBody2D
			std::span<ColliderHit2D> results, float distance = Math::Infinity) const;
		int Overlap(const Vector2f& position, float angle, const ContactFilter2D& contactFilter,									// Get a list of all Colliders that overlap all Colliders attached to this RigidBody2D
			std::vector<Collider2D*>& results) const;
		int GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const;							// Retrieves all contact points for all of the Collider(s) attached to this RigidBody2D
		int GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const;					// Retrieves all colliders in contact with this RigidBody2D, with the results filtered by the contactFilter
	};
}