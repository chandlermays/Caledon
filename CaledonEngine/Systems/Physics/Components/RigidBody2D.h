/*------------------------------
| File: RigidBody2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/LayerMask.h"
#include "Systems/Physics/Materials/PhysicsMaterial2D.h"
#include "Systems/Physics/Types/BodyConstraints2D.h"
#include "Systems/Physics/Types/BodyInterpolation2D.h"
#include "Systems/Physics/Types/BodySleepMode2D.h"
#include "Systems/Physics/Types/BodyType2D.h"
#include "Systems/Physics/Types/ColliderDistance2D.h"
#include "Systems/Physics/Types/CollisionDetectionMode2D.h"
#include "Systems/Physics/Types/ForceMode2D.h"
#include "Systems/Physics/Types/SlideConfig2D.h"
#include "Systems/Physics/Types/SlideResults2D.h"
#include "Systems/Scene/Component.h"
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
	class PhysicsScene2D;
	struct ColliderHit2D;
	struct ContactFilter2D;
	struct ContactPoint2D;
	class PhysicsShapeGroup2D;

	class RigidBody2D : public Component
	{
	private:
		PhysicsScene2D* m_pPhysicsScene;																															// The PhysicsScene2D this RigidBody2D is registered with
		BodyType2D m_bodyType;																																		// The physical behavior type of this RigidBody2D
		bool m_simulated;																																			// Indicates whether the RigidBody2D should be simulated or not by the physics system
		BodyConstraints2D m_constraints;																															// Controls which degrees of freedom are allowed for the simulation of this RigidBody2D (freezeRotation is derived from this)
		CollisionDetectionMode2D m_collisionDetectionMode;																											// The method used by the physics engine to check if two objects have collided
		BodyInterpolation2D m_interpolation;																														// Physics interpolation used between updates
		bool m_useFullKinematicContacts;																															// Should kinematic/kinematic and kinematic/static collisions be allowed?
		BodySleepMode2D m_sleepMode;																																// The sleep state that the RigidBody2D will initially be in
		bool m_isAwake;																																				// The current sleep state (true = awake)

		Vector2f m_position;																																		// The position of the RigidBody2D
		float m_rotation;																																			// The rotation of the RigidBody2D in degrees
		// Matrix4x4 m_localToWorldMatrix;																															// The transformation matrix used to transform the RigidBody2D to world space

		Vector2f m_linearVelocity;																																	// The linear velocity of the RigidBody2D in world-units per second (the X and Y components are derived from this)
		float m_angularVelocity;																																	// Angular velocity in degrees per second
		float m_linearDamping;																																		// The linear damping of the RigidBody2D linear velocity
		float m_angularDamping;																																		// The angular damping of this RigidBody2D angular velocity
		float m_gravityScale;																																		// The degree to which this object is affected by gravity
		Vector2f m_totalForce;																																		// The total amount of force that has been explicitly applied to this RigidBody2D since the last physics simulation step
		float m_totalTorque;																																		// The total amount of torque that has been explicitly applied to this RigidBody2D since the last physics simulation step

		bool m_useAutoMass;																																			// Should the total RigidBody2D mass be automatically calculated from the Collider2D.density of attached colliders?
		float m_mass;																																				// Mass of the RigidBody2D
		Vector2f m_centerOfMass;																																	// The center of mass of this RigidBody2D in local space
		float m_inertia;																																			// The RigidBody's resistance to changes in angular velocity (rotation)

		std::vector<Collider2D*> m_pAttachedColliders;																												// The Collider2D attached to this RigidBody2D (non-owning; the count is the size)
		PhysicsMaterial2D m_sharedMaterial;																															// The PhysicsMaterial2D that is applied to all Collider2D attached to this RigidBody2D
		LayerMask m_includeLayers;																																	// The additional Layers that all Collider2D attached to this RigidBody2D should include when deciding if a contact with another Collider2D should happen or not
		LayerMask m_excludeLayers;																																	// The additional Layers that all Collider2D attached to this RigidBody2D should exclude when deciding if a contact with another Collider2D should happen or not

		void WakeIfNeeded() noexcept;																																// Forces and velocity changes wake a sleeping body
		bool CanReceiveForces() const noexcept { return m_bodyType == BodyType2D::Dynamic && m_simulated; }															// Only simulated Dynamic bodies respond to forces

	public:
		RigidBody2D();																																				// Constructor
		~RigidBody2D() override;																																	// Destructor
		RigidBody2D(const RigidBody2D&) = delete;																													// Prevents copy-construction
		RigidBody2D& operator=(const RigidBody2D&) = delete;																										// Prevents copy-assignment
		RigidBody2D(RigidBody2D&&) = delete;																														// Prevents move-construction
		RigidBody2D& operator=(RigidBody2D&&) = delete;																												// Prevents move-assignment

		bool Initialize() override;																																	// Prepares this RigidBody2D for use
		const std::string& GetTypeName() const override;																											// Returns the type name of this Component

		PhysicsScene2D* GetPhysicsScene() const noexcept							{ return m_pPhysicsScene; }														// Returns the PhysicsScene2D this RigidBody2D is registered with
		BodyType2D GetBodyType() const noexcept										{ return m_bodyType; }															// Returns the BodyType for this RigidBody2D
		void SetBodyType(BodyType2D type) noexcept;																													// Sets the BodyType for this RigidBody2D
		bool IsSimulated() const noexcept											{ return m_simulated; }															// Returns whether this RigidBody2D is simulated by the physics system
		void SetSimulated(bool simulated) noexcept									{ m_simulated = simulated; }													// Sets whether this RigidBody2D is simulated by the physics system
		BodyConstraints2D GetConstraints() const noexcept							{ return m_constraints; }														// Returns the constraints restricting motion for this RigidBody2D
		void SetConstraints(BodyConstraints2D constraints) noexcept					{ m_constraints = constraints; }												// Sets the constraints restricting motion for this RigidBody2D
		bool GetFreezeRotation() const noexcept										{ return HasFlag(m_constraints, BodyConstraints2D::FreezeRotation); }			// Returns whether rotation is frozen for this RigidBody2D
		void SetFreezeRotation(bool freeze) noexcept;																												// Sets whether rotation is frozen for this RigidBody2D
		CollisionDetectionMode2D GetCollisionDetectionMode() const noexcept			{ return m_collisionDetectionMode; }											// Returns the collision detection mode used by this RigidBody2D
		void SetCollisionDetectionMode(CollisionDetectionMode2D mode) noexcept		{ m_collisionDetectionMode = mode; }											// Sets the collision detection mode used by this RigidBody2D
		BodyInterpolation2D GetInterpolation() const noexcept						{ return m_interpolation; }														// Returns the physics interpolation mode for this RigidBody2D
		void SetInterpolation(BodyInterpolation2D interpolation) noexcept			{ m_interpolation = interpolation; }											// Sets the physics interpolation mode for this RigidBody2D
		bool GetUseFullKinematicContacts() const noexcept							{ return m_useFullKinematicContacts; }											// Returns whether full kinematic-to-kinematic/static contact callbacks are enabled
		void SetUseFullKinematicContacts(bool use) noexcept							{ m_useFullKinematicContacts = use; }											// Sets whether full kinematic contacts are enabled

		BodySleepMode2D GetSleepMode() const noexcept								{ return m_sleepMode; }															// Returns the SleepMode configuration for this RigidBody2D
		void SetSleepMode(BodySleepMode2D mode) noexcept							{ m_sleepMode = mode; }															// Sets the SleepMode configuration for this RigidBody2D
		bool IsAwake() const noexcept												{ return m_isAwake; }															// Is this RigidBody2D 'Awake'?
		bool IsSleeping() const noexcept											{ return !m_isAwake; }															// Is this RigidBody2D 'Sleeping'?
		void Sleep();																																				// Make this RigidBody2D 'sleep' (ignored for NeverSleep bodies)
		void WakeUp() noexcept														{ m_isAwake = true; }															// Disables the 'sleeping' state of this RigidBody2D

		const Vector2f& GetPosition() const noexcept								{ return m_position; }															// Returns the position of this RigidBody2D
		void SetPosition(const Vector2f& position) noexcept							{ m_position = position; WakeIfNeeded(); }										// Sets the position of this RigidBody2D to the given position
		float GetRotation() const noexcept											{ return m_rotation; }															// Returns the rotation of this RigidBody2D
		void SetRotation(float angle);																																// Sets the rotation of this RigidBody2D to the given angle
		void MovePosition(const Vector2f& position);																												// Moves this RigidBody2D to the given position
		void MovePositionAndRotation(const Vector2f& position, float angle);																						// Moves this RigidBody2D to the given position and rotation angle
		void MoveRotation(float angle);																																// Moves this RigidBody2D to the given rotation angle
		SlideResults2D Slide(const Vector2f& velocity, float deltaTime, const SlideConfig2D& slideConfig);															// Slide this RigidBody2D using the given velocity integrated over deltaTime using the specified SlideConfig
		void SyncFromTransform();																																	// Copies the owner's Transform pose into the body (called by the physics subsystem before a step)
		void ApplyToTransform() const;																																// Writes the body pose back to the owner's Transform (called by the physics subsystem after a step)

		const Vector2f& GetLinearVelocity() const noexcept							{ return m_linearVelocity; }													// Returns the linear velocity of this RigidBody2D
		void SetLinearVelocity(const Vector2f& velocity) noexcept;																									// Sets the linear velocity of this RigidBody2D
		float GetLinearVelocityX() const noexcept									{ return m_linearVelocity.x; }													// Returns the X component of the linear velocity
		void SetLinearVelocityX(float x) noexcept									{ SetLinearVelocity(Vector2f{ x, m_linearVelocity.y }); }						// Sets the X component of the linear velocity
		float GetLinearVelocityY() const noexcept									{ return m_linearVelocity.y; }													// Returns the Y component of the linear velocity
		void SetLinearVelocityY(float y) noexcept									{ SetLinearVelocity(Vector2f{ m_linearVelocity.x, y }); }						// Sets the Y component of the linear velocity
		float GetAngularVelocity() const noexcept									{ return m_angularVelocity; }													// Returns the angular velocity of this RigidBody2D
		void SetAngularVelocity(float velocity) noexcept;																											// Sets the angular velocity of this RigidBody2D
		float GetLinearDamping() const noexcept										{ return m_linearDamping; }														// Returns the linear damping factor for this RigidBody2D
		void SetLinearDamping(float damping) noexcept								{ m_linearDamping = damping; }													// Sets the linear damping factor for this RigidBody2D
		float GetAngularDamping() const noexcept									{ return m_angularDamping; }													// Returns the angular damping factor for this RigidBody2D
		void SetAngularDamping(float damping) noexcept								{ m_angularDamping = damping; }													// Sets the angular damping factor for this RigidBody2D
		float GetGravityScale() const noexcept										{ return m_gravityScale; }														// Returns the gravity scale factor affecting this RigidBody2D
		void SetGravityScale(float scale) noexcept									{ m_gravityScale = scale; }														// Sets the gravity scale factor affecting this RigidBody2D

		const Vector2f& GetTotalForce() const noexcept								{ return m_totalForce; }														// Returns the total force accumulated on this RigidBody2D for the current step
		float GetTotalTorque() const noexcept										{ return m_totalTorque; }														// Gets the total torque accumulated on this RigidBody2D for the current step
		void ClearForces() noexcept													{ m_totalForce = Vector2f{ 0.0f, 0.0f }; m_totalTorque = 0.0f; }				// Called by the physics subsystem after each step
		
		void AddForce(const Vector2f& force, ForceMode2D mode = ForceMode2D::Force);																				// Apply a force to this RigidBody2D
		void AddForceAtPosition(const Vector2f& force, const Vector2f& position, ForceMode2D mode = ForceMode2D::Force);											// Apply a force at a given position in world-space
		void AddForceX(float force, ForceMode2D mode = ForceMode2D::Force);																							// Adds a force to the X component of the linear velocity only leaving the Y component of the world-space untouched
		void AddForceY(float force, ForceMode2D mode = ForceMode2D::Force);																							// Adds a force to the Y component of the linear velocity only leaving the X component of the world-space untouched
		void AddRelativeForce(const Vector2f& relativeForce, ForceMode2D mode = ForceMode2D::Force);																// Adds a force to the local-space linear velocity
		void AddRelativeForceX(float force, ForceMode2D mode = ForceMode2D::Force);																					// Adds a force to the X component of the linear velocity only leaving the Y component of the local-space untouched
		void AddRelativeForceY(float force, ForceMode2D mode = ForceMode2D::Force);																					// Adds a force to the Y component of the linear velocity only leaving the X component of the local-space untouched
		void AddTorque(float torque, ForceMode2D mode = ForceMode2D::Force);																						// Apply a torque at this RigidBody2D's center of mass

		bool GetUseAutoMass() const noexcept										{ return m_useAutoMass; }														// Returns whether mass is automatically calculated from attached colliders
		void SetUseAutoMass(bool useAutoMass);																														// Sets whether mass is automatically calculated from attached colliders
		float GetMass() const noexcept												{ return m_mass; }																// Returns the mass of this RigidBody2D
		void SetMass(float mass) noexcept;																															// Sets the mass of this RigidBody2D
		const Vector2f& GetCenterOfMass() const noexcept							{ return m_centerOfMass; }														// Returns the local-space center of mass for this RigidBody2D
		void SetCenterOfMass(const Vector2f& centerOfMass) noexcept					{ m_centerOfMass = centerOfMass; }												// Sets the local-space center of mass for this RigidBody2D
		Vector2f GetWorldCenterOfMass() const noexcept;																												// Returns the world-space center of mass for this RigidBody2D
		float GetInertia() const noexcept											{ return m_inertia; }															// Returns the rotational inertia of this RigidBody2D
		void SetInertia(float inertia) noexcept										{ m_inertia = inertia; }														// Sets the rotational inertia of this RigidBody2D
		void UpdateMassData();																																		// Computes the mass, center of mass, and inertia from the attached colliders (when useAutoMass = true)

		Vector2f GetPoint(const Vector2f& point) const;																												// Return a local space point given the point in rigidBody global space
		Vector2f GetVector(const Vector2f& vector) const;																											// Return a local space vector given the vector in rigidBody global space
		Vector2f GetRelativePoint(const Vector2f& relativePoint) const;																								// Return a global space point given the point relativePoint in rigidBody local space
		Vector2f GetRelativeVector(const Vector2f& relativeVector) const;																							// Return a global space vector given the vector relativeVector in rigidBody local space
		Vector2f GetPointVelocity(const Vector2f& point) const;																										// The velocity of the RigidBody2D at a point in world space
		Vector2f GetRelativePointVelocity(const Vector2f& relativePoint) const;																						// The velocity of the RigidBody2D at a point in its local space

		int GetAttachedColliderCount() const noexcept								{ return static_cast<int>(m_pAttachedColliders.size()); }						// Returns the total number of colliders attached to this RigidBody2D
		
		int GetAttachedColliders(std::vector<Collider2D*>& results, bool findTriggers = true) const;																// Returns all Collider2D that are attached to this RigidBody2D
		void RefreshAttachedColliders();																															// Re-scans the owning GameObject for Collider2D (children are not supported yet)
		bool AttachCollider(Collider2D* pCollider);																													// Attaches a Collider2D; returns false if it failed
		bool DetachCollider(Collider2D* pCollider);																													// Detaches a Collider2D; returns false if it failed
		const PhysicsMaterial2D& GetSharedMaterial() const noexcept					{ return m_sharedMaterial; }													// Returns the shared physical material applied to attached colliders
		void SetSharedMaterial(const PhysicsMaterial2D& material);																									// Applies the material to every attached Collider2D
		const LayerMask& GetIncludeLayers() const noexcept							{ return m_includeLayers; }														// Returns the layers included by default for contacts
		void SetIncludeLayers(LayerMask layers) noexcept							{ m_includeLayers = layers; }													// Sets the layers included by default for contacts
		const LayerMask& GetExcludeLayers() const noexcept							{ return m_excludeLayers; }														// Returns the layers excluded by default for contacts
		void SetExcludeLayers(LayerMask layers) noexcept							{ m_excludeLayers = layers; }													// Sets the layers excluded by default for contacts

		Vector2f ClosestPoint(const Vector2f& position) const;																										// Returns a point on the perimeter of all enabled Colliders attached to this RigidBody that is closest to the specified position
		bool OverlapPoint(const Vector2f& point) const;																												// Check if any of the RigidBody2D colliders overlap a given point in world-space
		int GetShapes(PhysicsShapeGroup2D& physicsShapeGroup) const;																								// Returns all the PhysicsShape2D used by all Collider2D attached to this RigidBody2D
		ColliderDistance2D Distance(const Collider2D* pCollider) const;																								// Calculates the minimum distance of this collider against all Collider2D attached to this RigidBody2D
		bool IsTouching(const Collider2D* pCollider) const;																											// Checks whether the collider is touching any of the collider(s) attached to this RigidBody2D or not
		bool IsTouchingLayers(LayerMask layerMask = LayerMask{ -1 }) const;																							// Checks whether any of the collider(s) attached to this RigidBody2D are touching any colliders on the specified layerMask or not
		int Cast(const Vector2f& direction, const ContactFilter2D& contactFilter,																					// All the Collider2D shapes attached to this RigidBody2D are cast into the Scene starting at each Collider position ignoring the Colliders attached to the same RigidBody2D
			std::span<ColliderHit2D> results, float distance = Math::Infinity) const;
		
		int Overlap(const Vector2f& position, float angle, const ContactFilter2D& contactFilter,																	// Returns a list of all Colliders that overlap all Colliders attached to this RigidBody2D
			std::vector<Collider2D*>& results) const;
		
		int GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const;															// Returns all contact points for all of the Collider(s) attached to this RigidBody2D
		int GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const;													// Returns all colliders in contact with this RigidBody2D, with the results filtered by the contactFilter
	};
}