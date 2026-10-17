/*------------------------------
| File: Collider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Scene/Component.h"

#include "Systems/Physics/Materials/PhysicsMaterial2D.h"
#include "Systems/Physics/Materials/PhysicsMaterialCombine2D.h"
#include "Systems/Physics/Types/ColliderDistance2D.h"
#include "Utilities/Geometry/AABB2D.h"

// The parent class for collider types used with 2D gameplay. Provides methods to define the shape and physical behavior
// for 2D object interactions, used to detect collisions, and trigger events in 2D game environments.

// The Collider2D class serves as a parent class for various two-dimenstional collider components that efine the physical boundaries of objects.
// These colliders enable interaction with other objects in a 2D physics simulation. They play a key role in detecting collisions and managing
// interactions, and setting up triggers for events when objects overlap or make contact with each other.

// To use these collider components, add them to GameObjects and configure them to match physical boundaries. While they are not necessary for static
// or trigger objects, you can add a Rigidbody2D components to enable dynamic interactions and movement. The recommended best practice is to properly
// configure size, position, and type because it is crucial to avoid inaccurate collision detefction or unintended behavior.

// These collider components work with Caledon's Physics2D system. They can be used with layers to optimize collision management and interactions.
// They also facilitate efficient overlap and raycast operations, such as Physics2D.OverlapCircle and Physics2D.Raycast, for precise object detection.

namespace CaledonEngine
{
	class Collider2D : public Component
	{
	private:
		RigidBody2D m_attachedRigidBody;															// The RigidBody2D attached to this Collider2D
		PhysicsMaterialCombine2D m_bounceCombine;													// The bounciness combine mode used by this Collider2D
		float m_bounciness;																			// The bounciness used by this Collider2D
		AABB2D m_bounds;																			// The world-space bounding area of this Collider2D
		float m_density;																			// The density of this Collider2D used to calculate its mass (when auto mass is enabled)
		float m_friction;																			// The friction used by this Collider2D
		PhysicsMaterialCombine2D m_frictionCombine;													// The friction combine mode used by this Collider2D
		bool m_isTrigger;																			// Is this collider configured as a trigger?
		Vector2f m_offset;																			// The local offset of the collider geometry
		PhysicsMaterial2D m_sharedMaterial;															// The PhysicsMaterial2D that is applied to this Collider2D


		// Properties to add
		//------------------------
		// LayerMask m_callbackLayers;																// The Layers that this Collider2D will report collision or trigger callbacks for during a contact with another Collider2D
		// CompositeCollider2D m_composite;															// Get the CompositeCollider2D that is available to be attached to this Collider2D
		// bool m_compositeCapable;																	// Indicates if this Collider2D is capable of being composited by the CompositeCollider2D
		// CompositeOperation m_compositeOperation;													// The composite operation to be used by a CompositeCollider2D
		// int m_compositeOrder;																	// The composite operation order to be used by a CompositeCollider2D
		// LayerMask m_contactCaptureLayers;														// The layers of other Collider2D involved in contacts with this Collider2D that will be captured
		// LayerMask m_contactMask;																	// Calculates the effective LayerMask that the Collider2D will use when determining if it can contact another Collider2D
		// ColliderErrorState2D m_errorState;														// The error state that indicates the state of the physics shapes the Collider2D tried to create
		// LayerMask m_excludeLayers;																// The additional Layers that this Collider2D should exclude when decidiing if a contact with another Collider2D should happen or not
		// LayerMask m_forceReceiveLayers;															// The Layers that this Collider2D can receive forces from during a Collision contact with another Collider2D
		// LayerMask m_forceSendLayers;																// The Layers that this Collider2D is allowed to send forces to during a Collision contact with another Collider2D
		// LayerMask m_includeLayers;																// The additional Layers that this Collider2D should include when decidiing if a contact with another Collider2D should happen or not
		// int m_layerOverridePriority;																// A decision priority assigned to this Collider2D used when there is a conflicting decision on whether a contact between itself and another Collision2D should happen or not
		// Matrix4x4 m_localToWorldMatrix;															// The transformation matrix used to transform the Collider physics shapes to world-space
		// int m_shapeCount;																		// The number of active PhysicsShape2D this Collider2D is currently using

	public:
		Collider2D();																				// Constructor
		~Collider2D() override = default;															// Destructor
		Collider2D(const Collider2D&) = delete;														// Prevent copy-construction
		Collider2D& operator=(const Collider2D&) = delete;											// Prevent copy-assignment
		Collider2D(Collider2D&&) = delete;															// Prevent move-construction
		Collider2D& operator=(Collider2D&&) = delete;												// Prevent move-assignment

		const std::string& GetTypeName() const override;											// Returns the type name of this Component

		bool CanContact(Collider2D collider);														// Determines if both Collider2D can ever come into contact
		Vector2f ClosestPoint(Vector2f position);													// Returns a point on the perimeter of this Collider2D that is closest to the given position
		ColliderDistance2D Distance(Collider2D collider);											// Calculates the minimum seperation of this Collider2D against another Collider2D
		bool IsTouching(Collider2D collider);														// Check whether this Collider2D is touching the Collider2D or not
		bool OverlapPoint(Vector2f point);															// Check if a Collider2D overlaps a given point in world-space

		
		// Public Methods to add
		//------------------------
		// (which uint should I use) uint GetShapeHash();														// Generates a simple hash value based upon the geometry of the Collider2D
		// OnCollisionEnter2D(Collision2D)																		// Sent when an incoming collider makes contact with this object's collider
		// OnCollisionExit2D(Collision2D)																		// Sent when a collider on another object stops touching this object's collider
		// OnCollisionStay2D(Collision2D)																		// Sent each frame where a collider on another object is touching this object's collider
		// OnTriggerEnter2D(Collision2D)																		// Sent when another object enters a trigger collider attached to this object
		// OnTriggerExit2D(Collision2D)																			// Send when another object leaves a trigger collider attached to this object
		// OnTriggerStay2D(Collision2D)																			// Sent each frame where another object is within a trigger collider attached to this object
		// NativeArray<RaycastHit2D> Cast(Vector2 direction, ContactFilter2D contactFilter,						// Casts the Collider shape into the Scene starting at the Collider position ignoring the Collider itself
		// float distance = Math::INFINITY, bool ignoreSiblingColliders = true, Allocator allocator);
		// Mesh CreateMesh(bool useBodyPosition, bool useBodyRotation, bool useDelaunay = true);				// Creates a planar Mesh that is identical to the area defined by the Collider2D geometry
		// ColliderArray2D GetContactColliders(ContactFilter2D contactFilter, Allocator allocator);
		// NativeArray<ContactPoint2D> GetContacts(ContactFilter2D contactFilter, Allocator allocator);			// Retrieves all contact points for this Collider
		// AABB2D GetShapeBounds(List<AABB2D> bounds, bool useRadii, bool useWorldSpace);						// Retrieves a list of Bounds for all PhysicsShape2D created by this Collider2D, and returns the combined Bounds of the retrieved list
		// int GetShapes(PhysicsShapeGroup2D physicsShapeGroup);												// Gets all the PhysicsShape2D used by the Collider2D
		// bool IsTouchingLayers(int layerMask = Physics2D.AllLayers);											// Checks whether this collider is touching any colliders on the specified layerMask or not
		// int Overlap(Vector2 position, float angle, List<Collider2D> results);								// Get a list of all Colliders that overlap this Collider
		// int Raycast(Vector2 direction, RaycastHit2D[] results, float distance = Math::INFINITY,				// Casts a ray into the Scene that starts at the Collider position and ignores the Collider itself
		//				int layerMask = Physics2D.AllLayers, float minDepth = -Math::INFINITY,
		//				float maxDepth = Math::INFINITY);
	};
}