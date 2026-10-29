/*------------------------------
| File: PhysicsScene2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Physics2DSettings.h"
#include "Systems/Physics/Queries/ColliderHit2D.h"
#include "Systems/Physics/Queries/ContactFilter2D.h"
#include "Systems/Physics/Types/CapsuleDirection2D.h"
#include "Systems/Physics/Types/ColliderDistance2D.h"
#include "Utilities/Math/Ray2D.h"
#include "Utilities/Math/Vector2.h"

#include <span>
#include <vector>

// Owns all 2D physics component objects added to it and can perform a simulation of them as well as execute queries against them
// All of this functionality is isolated from any other physics Scene. Using this, multiple independent physics Scenes can coexist

namespace CaledonEngine
{
	class Collider2D;
	class RigidBody2D;

	class PhysicsScene2D final
	{
	private:
		std::vector<Collider2D*> m_registeredColliders;																						// Collection of all active Collider2Ds registered to this PhysicsScene2D
		std::vector<RigidBody2D*> m_registeredBodies;																						// Collection of all active RigidBody2Ds registered to this PhysicsScene2D

		std::vector<std::pair<Collider2D*, Collider2D*>> m_ignoredPairs;																	// Collider pairs whose collisions are to be ignored
		const Physics2DSettings* m_pSettings;																								// The shared physics settings

		int m_subStepCount;																													// The number of simulation sub-steps that occurred during the last physics step
		float m_subStepLostTime;																											// The amount of simulation time lost due to hitting the max allowed sub-steps
		bool m_isValid;																														// Indicates whether this PhysicsScene2D is valid and operational

	public:
		explicit PhysicsScene2D(const Physics2DSettings& settings);																			// Constructor
		~PhysicsScene2D() = default;																										// Destructor
		PhysicsScene2D(const PhysicsScene2D&) = delete;																						// Prevent copy-construction
		PhysicsScene2D& operator=(const PhysicsScene2D&) = delete;																			// Prevent copy-assignment
		PhysicsScene2D(PhysicsScene2D&&) = delete;																							// Prevent move-construction
		PhysicsScene2D& operator=(PhysicsScene2D&&) = delete;																				// Prevent move-assignment

		const Physics2DSettings& GetSettings() const noexcept	{ return *m_pSettings; }													// Returns the shared physics settings this PhysicsScene2D simulates with

		int GetSubStepCount() const noexcept					{ return m_subStepCount; }													// Returns the number of simulation sub-steps from the last step
		float GetSubStepLostTime() const noexcept				{ return m_subStepLostTime; }												// Returns the amount of simulation time lost during sub-stepping

		int GetColliderCount() const noexcept					{ return static_cast<int>(m_registeredColliders.size()); }					// Returns the total number of registered Collider2D in this PhysicsScene2D
		int GetBodyCount() const noexcept						{ return static_cast<int>(m_registeredBodies.size()); }						// Returns the total number of registered RigidBody2D in this PhysicsScene2D

		bool IsEmpty() const noexcept							{ return m_registeredColliders.empty() && m_registeredBodies.empty(); }		// Checks whether this PhysicsScene2D contains any Collider2D or RigidBody2D
		bool IsValid() const noexcept							{ return m_isValid; }														// Checks whether this PhysicsScene2D is valid and active
		void Invalidate() noexcept								{ m_isValid = false; }														// Marks this PhysicsScene2D as invalid

		bool AddCollider(Collider2D* pCollider);																							// Adds a Collider2D to this PhysicsScene2D
		bool RemoveCollider(Collider2D* pCollider);																							// Removes a Collider2D from this PhysicsScene2D
		bool AddRigidBody2D(RigidBody2D* pRigidBody2D);																						// Adds a RigidBody2D to this PhysicsScene2D
		bool RemoveRigidBody2D(RigidBody2D* pRigidBody2D);																					// Removes a RigidBody2D from this PhysicsScene2D
		void WakeAllBodies() noexcept;																										// Wakes every registered RigidBody2D in this PhysicsScene2D

		bool GetIgnoreCollision(const Collider2D* pCollider1, const Collider2D* pCollider2) const;											// Checks whether collision between two Collider2D are to be ignored
		void SetIgnoreCollision(Collider2D* pCollider1, Collider2D* pCollider2, bool ignore = true);										// Sets a collision between two Collider2D to be ignored (or not)

		/*---------------------------------------------------------------------------------------------------------
		| --- BoxCast: Returns all intersections of a casted Box against the Colliders in this PhysicsScene2D --- |
		---------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const;
		int BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const;

		/*--------------------------------------------------------------------------------------------------------------
		| --- BoxCastAll: Returns all of the unique Colliders intersected by the casted Box in this PhysicsScene2D --- |
		--------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> BoxCastAll(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction,
			float distance = Math::Infinity, int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*----------------------------------------------------------------------------------------------------------------
		| --- CapsuleCast: Returns all intersections of a casted Capsule against the Colliders in this PhysicsScene2D --- |
		----------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity) const;
		int CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance = Math::Infinity) const;

		/*----------------------------------------------------------------------------------------------------------------------
		| --- CapsuleCastAll: Returns all of the unique Colliders intersected by the casted Capsule in this PhysicsScene2D --- |
		----------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> CapsuleCastAll(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle,
			const Vector2f& direction, float distance = Math::Infinity, int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*---------------------------------------------------------------------------------------------------------------
		| --- CircleCast: Returns all intersections of a casted Circle against the Colliders in this PhysicsScene2D --- |
		---------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity) const;
		int CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance = Math::Infinity) const;

		/*--------------------------------------------------------------------------------------------------------------------
		| --- CircleCastAll: Returns all of the unique Colliders intersected by the casted Circle in this PhysicsScene2D --- |
		--------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> CircleCastAll(const Vector2f& origin, float radius, const Vector2f& direction, float distance = Math::Infinity,
			int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*---------------------------------------------------------------------------------------------------------
		| --- Raycast: Returns all intersections of a casted Ray against the Colliders in this PhysicsScene2D --- |
		---------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity) const;
		int Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance = Math::Infinity) const;

		/*--------------------------------------------------------------------------------------------------------------
		| --- RaycastAll: Returns all of the unique Colliders intersected by the casted Ray in this PhysicsScene2D --- |
		--------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> RaycastAll(const Vector2f& origin, const Vector2f& direction, float distance = Math::Infinity,
			int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*-------------------------------------------------------------------------------------------------------------------
		| --- Linecast: Returns all intersections of a casted Line segment against the Colliders in this PhysicsScene2D --- |
		-------------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter) const;
		int Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const;

		/*------------------------------------------------------------------------------------------------------------------------
		| --- LinecastAll: Returns all of the unique Colliders intersected by the casted Line segment in this PhysicsScene2D --- |
		------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> LinecastAll(const Vector2f& start, const Vector2f& end, int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*---------------------------------------------------------------------------------------------------------------
		| --- GetRayIntersection: Returns all intersections of a Ray2D against the Colliders in this PhysicsScene2D --- |
		---------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> GetRayIntersection(const Ray2D& ray, float distance, int layerMask = Physics2DConstants::kDefaultRaycastLayers) const;
		int GetRayIntersection(const Ray2D& ray, float distance, int layerMask, std::span<ColliderHit2D> results) const;

		/*------------------------------------------------------------------------------------------------------------------------
		| --- GetRayIntersectionAll: Returns all of the unique Colliders interesected against a Ray2D in this PhysicsScene2D --- |
		------------------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> GetRayIntersectionAll(const Ray2D& ray, float distance = Math::Infinity, int layerMask = Physics2DConstants::kDefaultRaycastLayers) const;

		/*--------------------------------------------------------------------------------------------------------------------
		| --- OverlapArea: Returns all intersections of an overlapping Area against the Colliders in this PhysicsScene2D --- |
		--------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter) const;
		int OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*------------------------------------------------------------------------------------------------------------------------
		| --- OverlapAreaAll: Returns all of the unique Colliders intersected by the overlapping Area in this PhysicsScene2D --- |
		------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapAreaAll(const Vector2f& pointA, const Vector2f& pointB, int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*------------------------------------------------------------------------------------------------------------------
		| --- OverlapBox: Returns all intersections of an overlapping Box against the Colliders in this PhysicsScene2D --- |
		------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter) const;
		int OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*----------------------------------------------------------------------------------------------------------------------
		| --- OverlapBoxAll: Returns all of the unique Colliders intersected by the overlapping Box in this PhysicsScene2D --- |
		----------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapBoxAll(const Vector2f& point, const Vector2f& size, float angle, int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*--------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Colliders in this PhysicsScene2D --- |
		--------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter) const;
		int OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCapsuleAll: Returns all of the unique Colliders intersected by the overlapping Capsule in this PhysicsScene2D --- |
		------------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCapsuleAll(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection,
			float angle, int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Colliders in this PhysicsScene2D --- |
		------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter) const;
		int OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*----------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCircleAll: Returns all of the unique Colliders intersected by the overlapping Circle in this PhysicsScene2D --- |
		----------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCircleAll(const Vector2f& point, float radius, int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*----------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCollider: Returns all intersections of an overlapping Collider against the Colliders in this PhysicsScene2D --- |
		----------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const;
		int OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*----------------------------------------------------------------------------------------------------------------------
		| --- OverlapPoint: Returns all intersections of an overlapping Point against the Colliders in this PhysicsScene2D --- |
		----------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter) const;
		int OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*--------------------------------------------------------------------------------------------------------------------------
		| --- OverlapPointAll: Returns all of the unique Colliders intersected by the overlapping Point in this PhysicsScene2D --- |
		--------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapPointAll(const Vector2f& point, int layerMask = Physics2DConstants::kDefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		Vector2f ClosestPoint(const Vector2f& position, const Collider2D* pCollider) const;													//
		ColliderDistance2D Distance(const Collider2D* pColliderA, const Collider2D* pColliderB) const;										//
		bool IsTouching(const Collider2D* pCollider1, const Collider2D* pCollider2) const;													//
		bool IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const;											// Check whether this Collider2D is touching the Collider2D or not
		bool IsTouching(const ContactFilter2D& contactFilter) const;																		// Check whether this Collider2D is touching the Collider2D or not
		bool IsTouchingLayers(LayerMask = LayerMask{ -1 }) const;																			// Check whether this Collider2D is touching any colliders on the specified LayerMask or not

		void SyncTransforms();																												// Pushes the Transform poses of all registered bodies into the physics bodies
		bool Simulate(float deltaTime, int simulationLayers = Physics2DConstants::kAllLayers);												// Simulate physics associated with this PhysicsScene2D.
	};
}