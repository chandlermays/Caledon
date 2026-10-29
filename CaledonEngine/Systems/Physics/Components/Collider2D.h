/*------------------------------
| File: Collider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Scene/Component.h"

#include "Systems/Physics/Materials/PhysicsMaterial2D.h"
#include "Systems/Physics/Materials/PhysicsMaterialCombine2D.h"
#include "Systems/Physics/Types/ColliderDistance2D.h"
#include "Systems/Physics/Types/ColliderErrorState2D.h"
#include "Systems/Physics/Types/CompositeOperation.h"
#include "Systems/Physics/LayerMask.h"
#include "Utilities/Geometry/AABB2D.h"
#include "Utilities/Math/Vector2.h"

#include <cstdint>
#include <functional>
#include <span>
#include <vector>

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
	class CompositeCollider2D;
	class RigidBody2D;
	class Collision2D;
	struct ColliderHit2D;
	struct ContactFilter2D;
	struct ContactPoint2D;
	class PhysicsScene2D;
	class PhysicsShapeGroup2D;

	class Collider2D : public Component
	{
	public:
		using CollisionCallback = std::function<void(Collider2D*)>;															// Type alias for a collision callback function that takes a pointer to another Collider2D as an argument

	protected:
		virtual int AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const;											//

	private:
		RigidBody2D* m_pAttachedRigidBody;																					// The RigidBody2D attached to this Collider2D
		PhysicsScene2D* m_pPhysicsScene2D;																					// The PhysicsScene2D this Collider2D is registered with
		PhysicsMaterialCombine2D m_bounceCombine;																			// The bounciness combine mode used by this Collider2D
		float m_bounciness;																									// The bounciness used by this Collider2D
		AABB2D m_bounds;																									// The world-space bounding area of this Collider2D
		float m_density;																									// The density of this Collider2D used to calculate its mass (when auto mass is enabled)
		float m_friction;																									// The friction used by this Collider2D
		PhysicsMaterialCombine2D m_frictionCombine;																			// The friction combine mode used by this Collider2D
		bool m_isTrigger;																									// Is this collider configured as a trigger?
		Vector2f m_offset;																									// The local offset of the collider geometry
		PhysicsMaterial2D m_sharedMaterial;																					// The PhysicsMaterial2D that is applied to this Collider2D

		std::vector<CollisionCallback> m_onEnterCallbacks;																	// Callbacks to invoke when a collision starts
		std::vector<CollisionCallback> m_onUpdateCallbacks;																	// Callbacks to invoke when a collision is ongoing
		std::vector<CollisionCallback> m_onExitCallbacks;																	// Callbacks to invoke when a collision ends

		LayerMask m_callbackLayers;																							// The Layers that this Collider2D will report collision or trigger callbacks for during a contact with another Collider2D
		LayerMask m_contactCaptureLayers;																					// The layers of other Collider2D involved in contacts with this Collider2D that will be captured
		LayerMask m_contactMask;																							// Calculates the effective LayerMask that the Collider2D will use when determining if it can contact another Collider2D
		LayerMask m_excludeLayers;																							// The additional Layers that this Collider2D should exclude when decidiing if a contact with another Collider2D should happen or not
		LayerMask m_forceReceiveLayers;																						// The Layers that this Collider2D can receive forces from during a Collision contact with another Collider2D
		LayerMask m_forceSendLayers;																						// The Layers that this Collider2D is allowed to send forces to during a Collision contact with another Collider2D
		LayerMask m_includeLayers;																							// The additional Layers that this Collider2D should include when decidiing if a contact with another Collider2D should happen or not

		CompositeCollider2D* m_pComposite;																					// Get the CompositeCollider2D that is available to be attached to this Collider2D
		bool m_compositeCapable;																							// Indicates if this Collider2D is capable of being composited by the CompositeCollider2D
		CompositeOperation m_compositeOperation;																			// The composite operation to be used by a CompositeCollider2D
		int m_compositeOrder;																								// The composite operation order to be used by a CompositeCollider2D
		ColliderErrorState2D m_errorState;																					// The error state that indicates the state of the physics shapes the Collider2D tried to create
		int m_layerOverridePriority;																						// A decision priority assigned to this Collider2D used when there is a conflicting decision on whether a contact between itself and another Collision2D should happen or not
		
		int m_shapeCount;																									// The number of active PhysicsShape2D this Collider2D is currently using
		
		// Properties to add
		//------------------------
		// Matrix4x4 m_localToWorldMatrix;																					// The transformation matrix used to transform the Collider physics shapes to world-space

	private:
		int BuildShapes(PhysicsShapeGroup2D& physicsShapeGroup, bool worldSpace) const;										//
		void GetWorldPose(Vector2f& position, float& rotation, Vector2f& scale) const;										//

	public:
		Collider2D();																										// Constructor
		~Collider2D() override;																								// Destructor
		Collider2D(const Collider2D&) = delete;																				// Prevent copy-construction
		Collider2D& operator=(const Collider2D&) = delete;																	// Prevent copy-assignment
		Collider2D(Collider2D&&) = delete;																					// Prevent move-construction
		Collider2D& operator=(Collider2D&&) = delete;																		// Prevent move-assignment

		bool Initialize() override;																							// Prepares this Collider2D for use
		const std::string& GetTypeName() const override;																	// Returns the type name of this Component

		bool CanContact(const Collider2D* pCollider) const;																	// Determines if both Collider2D can ever come into contact
		Vector2f ClosestPoint(const Vector2f& position) const;																// Returns a point on the perimeter of this Collider2D that is closest to the given position
		ColliderDistance2D Distance(const Collider2D* pCollider) const;														// Calculates the minimum seperation of this Collider2D against another Collider2D
		bool IsTouching(const Collider2D* pCollider) const;																	// Check whether this Collider2D is touching the Collider2D or not
		bool IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const;							// Check whether this Collider2D is touching the Collider2D or not
		bool IsTouching(const ContactFilter2D& contactFilter) const;														// Check whether this Collider2D is touching the Collider2D or not
		bool IsTouchingLayers(LayerMask = LayerMask{ -1 }) const;															// Check whether this Collider2D is touching any colliders on the specified LayerMask or not
		bool OverlapPoint(const Vector2f& point) const;																		// Check if a Collider2D overlaps a given point in world-space

		virtual void OnCollisionEnter2D(const Collision2D& collision);														// Sent when an incoming collider makes contact with this object's collider
		virtual void OnCollisionExit2D(const Collision2D& collision);														// Sent when a collider on another object stops touching this object's collider
		virtual void OnCollisionStay2D(const Collision2D& collision);														// Sent each frame where a collider on another object is touching this object's collider
		virtual void OnTriggerEnter2D(const Collision2D& collision);														// Sent when another object enters a trigger collider attached to this object
		virtual void OnTriggerExit2D(const Collision2D& collision);															// Sent when another object leaves a trigger collider attached to this object
		virtual void OnTriggerStay2D(const Collision2D& collision);															// Sent each frame where another object is within a trigger collider attached to this object
		
		RigidBody2D* GetAttachedRigidBody() const noexcept																	{ return m_pAttachedRigidBody; }					// Returns the RigidBody2D attached to this Collider2D
		void SetAttachedRigidBody(RigidBody2D* pBody) noexcept																{ m_pAttachedRigidBody = pBody; }					// Sets the RigidBody2D attached to this Collider2D

		PhysicsScene2D* GetPhysicsScene() const noexcept																	{ return m_pPhysicsScene2D; }						// Returns the PhysicsScene2D this Collider2D is registered with

		PhysicsMaterialCombine2D GetBounceCombine() const noexcept															{ return m_bounceCombine; }							// Returns the bounciness combine mode of this Collider2D
		void SetBounceCombine(PhysicsMaterialCombine2D combine) noexcept													{ m_bounceCombine = combine; }						// Sets the bounciness combine mode of this Collider2D
		float GetBounciness() const noexcept																				{ return m_bounciness; }							// Returns the bounciness value of this Collider2D
		void SetBounciness(float bounciness) noexcept																		{ m_bounciness = bounciness; }						// Sets the bounciness value of this Collider2D
		
		float GetDensity() const noexcept																					{ return m_density; }								// Returns the density value of this Collider2D
		void SetDensity(float density) noexcept																				{ m_density = density; }							// Sets the density value of this Collider2D
		
		float GetFriction() const noexcept																					{ return m_friction; }								// Returns the friction value of this Collider2D
		void SetFriction(float friction) noexcept																			{ m_friction = friction; }							// Sets the friction value of this Collider2D
		PhysicsMaterialCombine2D GetFrictionCombine() const noexcept														{ return m_frictionCombine; }						// Returns the friction combine mode of this Collider2D
		void SetFrictionCombine(PhysicsMaterialCombine2D combine) noexcept													{ m_frictionCombine = combine; }					// Sets the friction combine mode of this Collider2D
		
		bool IsTrigger() const noexcept																						{ return m_isTrigger; }								// Returns whether this Collider2D is a trigger or not
		void SetIsTrigger(bool isTrigger) noexcept																			{ m_isTrigger = isTrigger; }						// Sets whether this Collider2D is a trigger or not
		
		const Vector2f& GetOffset() const noexcept																			{ return m_offset; }								// Returns the local offset of this Collider2D's geometry
		void SetOffset(const Vector2f& offset) noexcept																		{ m_offset = offset; RefreshBounds(); }				// Sets the local offset of this Collider2D's geometry

		const PhysicsMaterial2D& GetSharedMaterial() const noexcept															{ return m_sharedMaterial; }						// Returns the shared physics material of this Collider2D
		void SetSharedMaterial(const PhysicsMaterial2D& material);																												// Sets the shared physics material of this Collider2D
		
		const LayerMask& GetCallbackLayer() const noexcept																	{ return m_callbackLayers; }						// Returns the callback layers that this Collider2D will use
		void SetCallbackLayers(LayerMask layers) noexcept																	{ m_callbackLayers = layers; }						// Sets the callback layers that this Collider2D will use
		const LayerMask& GetContactCaptureLayers() const noexcept															{ return m_contactCaptureLayers; }					// Returns the contact capture layers that this Collider2D will use
		void SetContactCaptureLayers(LayerMask layers) noexcept																{ m_contactCaptureLayers = layers; }				// Sets the contact capture layers that this Collider2D will use
		const LayerMask& GetExcludeLayers() const noexcept																	{ return m_excludeLayers; }							// Returns the exclude layers that this Collider2D will use
		void SetExcludeLayers(LayerMask layers) noexcept																	{ m_excludeLayers = layers; }						// Sets the exclude layers that this Collider2D will use
		const LayerMask& GetForceReceiveLayers() const noexcept																{ return m_forceReceiveLayers; }					// Returns the force receive layers that this Collider2D will use
		void SetForceReceiveLayers(LayerMask layers) noexcept																{ m_forceReceiveLayers = layers; }					// Sets the force receive layers that this Collider2D will use
		const LayerMask& GetForceSendLayers() const noexcept																{ return m_forceSendLayers; }						// Returns the force send layers that this Collider2D will use
		void SetForceSendLayers(LayerMask layers) noexcept																	{ m_forceSendLayers = layers; }						// Sets the force send layers that this Collider2D will use
		const LayerMask& GetIncludeLayers() const noexcept																	{ return m_includeLayers; }							// Returns the include layers that this Collider2D will use
		void SetIncludeLayers(LayerMask layers) noexcept																	{ m_includeLayers = layers; }						// Sets the include layers that this Collider2D will use
		const LayerMask& GetContactMask() const noexcept																	{ return m_contactMask; }							// Returns the effective contact mask that this Collider2D will use
		void SetContactMask(LayerMask mask) noexcept																		{ m_contactMask = mask; }							// Sets the effective contact mask that this Collider2D will use

		CompositeCollider2D* GetComposite() const noexcept																	{ return m_pComposite; }							// Returns the composite collider of this Collider2D
		void SetComposite(CompositeCollider2D* pComposite) noexcept															{ m_pComposite = pComposite; }						// Sets the composite collider of this Collider2D
		bool IsCompositeCapabale() const noexcept																			{ return m_compositeCapable; }						// Returns whether this Collider2D is capabale of being composited
		void SetCompositeCapabale(bool capabale) noexcept																	{ m_compositeCapable = capabale; }					// Sets whether this Collider2D is capabale of being composited
		CompositeOperation GetCompositeOperation() const noexcept															{ return m_compositeOperation; }					// Returns the composite operation of this Collider2D
		void SetCompositeOperation(CompositeOperation operation) noexcept													{ m_compositeOperation = operation; }				// Sets the composite operation of this Collider2D
		int GetCompositeOrder() const noexcept																				{ return m_compositeOrder; }						// Returns the composite order of this Collider2D
		void SetCompositeOrder(int order) noexcept																			{ m_compositeOrder = order; }						// Sets the composite order of this Collider2D
		
		ColliderErrorState2D GetErrorState() const noexcept																	{ return m_errorState; }							// Returns the shape creation error state of this Collider2D
		int GetLayerOverridePriority() const noexcept																		{ return m_layerOverridePriority; }					// Returns the layer override priority of this Collider2D
		void SetLayerOverridePriority(int priority) noexcept																{ m_layerOverridePriority = priority; }				// Sets the layer override priority of this Collider2D
		int GetShapeCount() const noexcept																					{ return m_shapeCount; }							// Returns the number of active shapes of this Collider2D
		const AABB2D& GetBounds() const noexcept																			{ return m_bounds; }								// Returns the world-space bounds of this Collider2D

		void RefreshBounds();																								// Refreshes the bounding area of this collider
		virtual int GetShapes(PhysicsShapeGroup2D& physicsSHapeGroup) const;												// Populates the shape group with physics shapes
		std::uint32_t GetShapesHash() const;																				// Generates a hash representing the current shapes
		AABB2D GetShapeBounds(std::vector<AABB2D>& bounds, bool useRadii, bool useWorldSpace) const;						// Returns bounds for all constituent shapes

		int Cast(const Vector2f& direction, const ContactFilter2D& contactFilter,											// 
			std::span<ColliderHit2D> results, float distance = Math::Infinity,
			bool ignoreSiblingColliders = true) const;

		int Overlap(const Vector2f& position, float angle,																	// 
			const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		int Raycast(const Vector2f& direction, std::span<ColliderHit2D> results,											// 
			float distance = Math::Infinity, LayerMask layerMask = LayerMask{ -1 },
			float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		int GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const;					// Retrieves contact points involving this Collider2D
		int GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const;			// Retrieves colliders currently in contact with this Collider2D

		// Methods to add
		//------------------------
		// Mesh CreateMesh(bool useBodyPos, bool useBodyRot, bool useDelaunay = true);											// Creates a renderable mesh representation of the collider shape
	};
}