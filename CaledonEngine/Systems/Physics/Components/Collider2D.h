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
	class PhysicsShapeGroup2D;

	class Collider2D : public Component
	{
	private:
		RigidBody2D* m_pAttachedRigidBody;																					// The RigidBody2D attached to this Collider2D
		PhysicsMaterialCombine2D m_bounceCombine;																			// The bounciness combine mode used by this Collider2D
		float m_bounciness;																									// The bounciness used by this Collider2D
		AABB2D m_bounds;																									// The world-space bounding area of this Collider2D
		float m_density;																									// The density of this Collider2D used to calculate its mass (when auto mass is enabled)
		float m_friction;																									// The friction used by this Collider2D
		PhysicsMaterialCombine2D m_frictionCombine;																			// The friction combine mode used by this Collider2D
		bool m_isTrigger;																									// Is this collider configured as a trigger?
		Vector2f m_offset;																									// The local offset of the collider geometry
		PhysicsMaterial2D m_sharedMaterial;																					// The PhysicsMaterial2D that is applied to this Collider2D

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

	public:
		Collider2D();																										// Constructor
		~Collider2D() override = default;																					// Destructor
		Collider2D(const Collider2D&) = delete;																				// Prevent copy-construction
		Collider2D& operator=(const Collider2D&) = delete;																	// Prevent copy-assignment
		Collider2D(Collider2D&&) = delete;																					// Prevent move-construction
		Collider2D& operator=(Collider2D&&) = delete;																		// Prevent move-assignment

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
		virtual void OnTriggerExit2D(const Collision2D& collision);															// Send when another object leaves a trigger collider attached to this object
		virtual void OnTriggerStay2D(const Collision2D& collision);															// Sent each frame where another object is within a trigger collider attached to this object
		
		RigidBody2D* GetAttachedRigidBody() const noexcept																	{ return m_pAttachedRigidBody; }
		void SetAttachedRigidBody(RigidBody2D* pBody) noexcept																{ m_pAttachedRigidBody = pBody; }

		PhysicsMaterialCombine2D GetBounceCombine() const noexcept															{ return m_bounceCombine; }
		void SetBounceCombine(PhysicsMaterialCombine2D combine) noexcept													{ m_bounceCombine = combine; }
		float GetBounciness() const noexcept																				{ return m_bounciness; }
		void SetBouniness(float bounciness) noexcept																		{ m_bounciness = bounciness; }
		
		float GetDensity() const noexcept																					{ return m_density; }
		void SetDensity(float density) noexcept																				{ m_density = density; }
		
		float GetFriction() const noexcept																					{ return m_friction; }
		void SetFriction(float friction) noexcept																			{ m_friction = friction; }
		PhysicsMaterialCombine2D GetFrictionCombine() const noexcept														{ return m_frictionCombine; }
		void SetFrictionCombine(PhysicsMaterialCombine2D combine) noexcept													{ m_frictionCombine = combine; }
		
		bool IsTrigger() const noexcept																						{ return m_isTrigger; }
		void SetIsTrigger(bool isTrigger) noexcept																			{ m_isTrigger = isTrigger; }
		
		const Vector2f& GetOffset() const noexcept																			{ return m_offset; }
		void SetOffset(const Vector2f& offset) noexcept																		{ m_offset = offset; }

		const PhysicsMaterial2D& GetSharedMaterial() const noexcept															{ return m_sharedMaterial; }
		void SetSharedMaterial(const PhysicsMaterial2D& material);
		
		const LayerMask& GetCallbackLayer() const noexcept																	{ return m_callbackLayers; }
		void SetCallbackLayers(LayerMask layers) noexcept																	{ m_callbackLayers = layers; }
		const LayerMask& GetContactCaptureLayers() const noexcept															{ return m_contactCaptureLayers; }
		void SetContactCaptureLayers(LayerMask layers) noexcept																{ m_contactCaptureLayers = layers; }
		const LayerMask& GetExcludeLayers() const noexcept																	{ return m_excludeLayers; }
		void SetExcludeLayers(LayerMask layers) noexcept																	{ m_excludeLayers = layers; }
		const LayerMask& GetForceReceiveLayers() const noexcept																{ return m_forceReceiveLayers; }
		void SetForceReceiveLayers(LayerMask layers) noexcept																{ m_forceReceiveLayers = layers; }
		const LayerMask& GetForceSendLayers() const noexcept																{ return m_forceSendLayers; }
		void SetForceSendLayers(LayerMask layers) noexcept																	{ m_forceSendLayers = layers; }
		const LayerMask& GetIncludeLayers() const noexcept																	{ return m_includeLayers; }
		void SetIncludeLayers(LayerMask layers) noexcept																	{ m_includeLayers = layers; }
		const LayerMask& GetContactMask() const noexcept																	{ return m_contactMask; }
		void SetContactMask(LayerMask mask) noexcept																		{ m_contactMask = mask; }

		CompositeCollider2D* GetComposite() const noexcept																	{ return m_pComposite; }
		void SetComposite(CompositeCollider2D* pComposite) noexcept															{ m_pComposite = pComposite; }
		bool IsCompositeCapabale() const noexcept																			{ return m_compositeCapable; }
		void SetCompositeCapabale(bool capabale) noexcept																	{ m_compositeCapable = capabale; }
		CompositeOperation GetCompositeOperation() const noexcept															{ return m_compositeOperation; }
		void SetCompositeOperation(CompositeOperation operation) noexcept													{ m_compositeOperation = operation; }
		int GetCompositeOrder() const noexcept																				{ return m_compositeOrder; }
		void SetCompositeOrder(int order) noexcept																			{ m_compositeOrder = order; }
		
		ColliderErrorState2D GetErrorState() const noexcept																	{ return m_errorState; }
		int GetLayerOverridePriority() const noexcept																		{ return m_layerOverridePriority; }
		void SetLayerOverridePriority(int priority) noexcept																{ m_layerOverridePriority = priority; }
		int GetShapeCount() const noexcept																					{ return m_shapeCount; }
		const AABB2D& GetBounds() const noexcept																			{ return m_bounds; }

		void RefreshBounds();																								//
		virtual int GetShapes(PhysicsShapeGroup2D& physicsSHapeGroup) const;												//
		std::uint32_t GetShapesHash() const;																				//
		AABB2D GetShapeBounds(std::vector<AABB2D>& bounds, bool useRadii, bool useWorldSpace) const;						//

		int Cast(const Vector2f& direction, const ContactFilter2D& contactFilter,											//
			std::span<ColliderHit2D> results, float distance = Math::Infinity,
			bool ignoreSiblingColliders = true) const;

		int Overlap(const Vector2f& position, float angle,																	//
			const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		int Raycast(const Vector2f& direction, std::span<ColliderHit2D> results,											//
			float distance = Math::Infinity, LayerMask layerMask = LayerMask{ -1 },
			float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		int GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const;					//
		int GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const;			//

		// Methods to add
		//------------------------
		// Mesh CreateMesh(bool useBodyPos, bool useBodyRot, bool useDelaunay = true);											//
	};
}