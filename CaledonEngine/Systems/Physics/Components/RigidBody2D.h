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

#include <memory>

// Provides physics movement and other dyanmics, and the ability to attach Collider2D to it

// This is a fundamental physics component that provides multiple simulation dynamics, such as RigidBody2D.position and RigidBody2D.rotation for pose control,
// and RigidBody2D.linearVelocity and RigidBody2D.angularVelocity for velocity control

// You can attach multiple Collider2D to a RigidBody2D to detect collisions and provide a collision response when you set RigidBody2D.bodyType to Dynamic

namespace CaledonEngine
{
	class Collider2D;

	class RigidBody2D : public Component
	{
	private:
		float m_angularDamping;															// the angular damping of this RigidBody2D angular velocity
		float m_angularVelocity;														// angular velocity in degrees per second
		int m_attachedColliderCount;													// returns the number of Collider2D attached to this RigidBody2D
		BodyType2D m_bodyType;															// the physical behavior type of this RigidBody2D
		Vector2f m_centerOfMass;														// the center of mass of this RigidBody2D in local space
		CollisionDetectionMode2D m_collisionDetectionMode;								// the method used by the physics engine to check if two objects have collided
		BodyConstraints2D m_constraints;												// controls which degrees of freedom are allowed for the simulation of this RigidBody2D
		LayerMask m_excludeLayers;														// the additional Layers that all Collider2D attached to this RigidBody2D should exclude when deciding if a contact with another Collider2D should happen or not
		bool m_freezeRotation;															// controls whether physics will change the rotation of the object
		float m_gravityScale;															// the degree to which this object is affected by gravity
		LayerMask m_includeLayers;														// the additional Layers that all Collider2D attached to this RigidBody2D should include when deciding if a contact with another Collider2D should happen or not
		float m_inertia;																// the RigidBody's resistance to changes in angular velocity (rotation)
		BodyInterpolation2D m_interpolation;											// Physics interpolation used between updates
		float m_linearDamping;															// the linear damping of the RigidBody2D linear velocity
		Vector2f m_linearVelocity;														// the linear velocity of the RigidBody2D represents the rate of change over time of the RigidBody2D position in world-units
		float m_linearVelocityX;														// the X component of the linear velocity of the RigidBody2D in world-units per second
		float m_linearVelocityY;														// the Y component of the linear velocity of the RigidBody2D in world-units per second
		// Matrix4x4 m_localToWorldMatrix;												// the transformation matrix used to transform the RigidBody2D to world space
		float m_mass;																	// mass of the RigidBody2D
		Vector2f m_position;															// the position of the RigidBody2D
		float m_rotation;																// the rotation of the RigidBody2D
		PhysicsMaterial2D m_sharedMaterial;												// the PhysicsMaterial2D that is applied to all Collider2D attached to this RigidBody2D
		bool m_simulated;																// indicates whether the RigidBody2D should be simulated or not by the physics system
		BodySleepMode2D m_sleepMode;													// the sleep state that the RigidBody2D will initially be in
		Vector2f m_totalForce;															// the total amount of force that has been explicitly applied to this RigidBody2D since the last physics simulation step
		float m_totalTorque;															// the total amount of torque that has been explicitly applied to this RigidBody2D since the last physics simulation step
		bool m_useAutoMass;																// should the total RigidBody2D mass be automatically calculated from the Collider2D.density of attached colliders?
		bool m_useFullKinematicContacts;												// should kinematic/kinematic and kinematic/static collisions be allowed?
		Vector2f m_worldCenterOfMass;													// gets the center of mass of the RigidBody2D in global space

	public:
		RigidBody2D();																												// Constructor
		~RigidBody2D() override = default;																							// Destructor
		RigidBody2D(const RigidBody2D&) = delete;																					// Prevents copy-construction
		RigidBody2D& operator=(const RigidBody2D&) = delete;																		// Prevents copy-assignment
		RigidBody2D(RigidBody2D&&) = delete;																						// Prevents move-construction
		RigidBody2D& operator=(RigidBody2D&&) = delete;																				// Prevents move-assignment

		const std::string& GetTypeName() const override;																			// Returns the type name of this Component

		void AddForce(Vector2f force, ForceMode2D mode = ForceMode2D::Force);														// Apply a force to this RigidBody2D
		void AddForceAtPosition(Vector2f force, Vector2f position, ForceMode2D mode = ForceMode2D::Force);							// Apply a force at a given position in world-space
		void AddForceX(float force, ForceMode2D mode = ForceMode2D::Force);															// Adds a force to the X component of the linear velocity only leaving the Y component of the world-space untouched
		void AddForceY(float force, ForceMode2D mode = ForceMode2D::Force);															// Adds a force to the Y component of the linear velocity only leaving the X component of the world-space untouched
		void AddRelativeForce(Vector2f relativeForce, ForceMode2D mode = ForceMode2D::Force);										// Adds a force to the local-space linear velocity
		void AddRelativeForceX(float force, ForceMode2D mode = ForceMode2D::Force);													// Adds a force to the X component of the linear velocity only leaving the Y component of the local-space untouched
		void AddRelativeForceY(float force, ForceMode2D mode = ForceMode2D::Force);													// Adds a force to the Y component of the linear velocity only leaving the X component of the local-space untouched
		void AddTorque(float torque, ForceMode2D mode = ForceMode2D::Force);														// Apply a torque at this RigidBody2D's center of mass
		Vector2f ClosestPoint(Vector2f position);																					// Returns a point on the perimeter of all enabled Colliders attached to this RigidBody that is closest to the specified position
		ColliderDistance2D Distance(Collider2D collider);																			// Calculates the minimum distance of this collider against all Collider2D attached to this RigidBody2D
		Vector2f GetPoint(Vector2f point);																							// 
		Vector2f GetPointVelocity(Vector2f point);																					//
		Vector2f GetRelativePoint(Vector2f relativePoint);																			//
		Vector2f GetRelativePointVelocity(Vector2f relativePoint);																	//
		Vector2f GetRelativeVector(Vector2f relativeVector);																		//
		Vector2f GetVector(Vector2f vector);																						// 
		bool IsAwake();																												// Is this RigidBody2D 'Awake'?
		bool IsSleeping();																											// Is this RigidBody2D 'Sleeping'?
		bool IsTouching(Collider2D collider);																						// Checks whether the collider is touching any of the collider(s) attached to this RigidBody2D or not
		void MovePosition(Vector2f position);																						// Moves this RigidBody2D to the given position
		void MovePositionAndRotation(Vector2f position, float angle);																// Moves this RigidBody2D to the given position and rotation angle
		void MoveRotation(float angle);																								// Moves this RigidBody2D to the given rotation angle
		bool OverlapPoint(Vector2f point);																							// Check if any of the RigidBody2D colliders overlap a given point in world-space
		void SetRotation(float angle);																								// Sets the rotation of this RigidBody2D to the given angle
		void Sleep();																												// Make this RigidBody2D 'sleep'
		SlideResults2D Slide(Vector2f velocity, float deltaTime, SlideConfig2D slideConfig);										// Slide this RigidBody2D using the given velocity integrated over deltaTime using the specified SlideConfig
		void WakeUp();																												// Disables the 'sleeping' state of this RigidBody2D
		
		//---------------------
		// (NOT YET) int Cast(Vector2f direction, List<RaycastHit2D> results, float distance = Math::INFINITY);						// All the Collider2D shapes attached to this RigidBody2D are cast into the Scene starting at each Collider position ignoring the Colliders attached to the same RigidBody2D
		// (NOT YET) int GetAttachedColliders(out Collider2D[] results, bool findTriggers = true);									// Returns all Collider2D that are attached to this RigidBody2D
		// (NOT YET) ColliderArray2D GetContactColliders(ContactFilter2D contactFilter, Allocator allocator);						// Retrieves all colliders in contact with this RigidBody2D, with the results filtered by the contactFilter
		// (NOT YET) NativeArray<ContactPoint2D> GetContacts(ContactFilter2D contactFilter, Allocator allocator);					// Retrieves all contact points for all of the Collider(s) attached to this RigidBody2D
		// (NOT YET) int GetShapes(PhysicsShapeGroup2D physicsShapeGroup);															// Gets all the PhysicsShape2D used by all Collider2D attached to this RigidBody2D
		// (NOT YET) bool IsTouchingLayers(int layerMask = Physics2D.AllLayers);													// Checks whether any of the collider(s) attached to this RigidBody2D are touching any colliders on the specified layerMask or not
		// (NOT YET) int Overlap(Vector2f position, float angle, List<Collider2D> results);											// Get a list of all Colliders that overlap all Colliders attached to this RigidBody2D
	};
}