/*------------------------------
| File: Physics2DSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Engine/IFixedUpdatable.h"
#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IUpdatable.h"
#include "Systems/Physics/Events/ContactPoint2D.h"
#include "Systems/Physics/Physics2DSettings.h"
#include "Systems/Physics/PhysicsScene2D.h"
#include "Systems/Physics/Queries/ColliderHit2D.h"
#include "Systems/Physics/Queries/ContactFilter2D.h"
#include "Systems/Physics/Types/CapsuleDirection2D.h"
#include "Systems/Physics/Types/ColliderDistance2D.h"
#include "Systems/Physics/Types/SimulationMode2D.h"
#include "Utilities/Math/Ray2D.h"
#include "Utilities/Math/Vector2.h"

#include <span>
#include <string_view>
#include <vector>

// Provides utilities and global settings to manage and simulate 2D physics interactions

namespace CaledonEngine
{
	class Collider2D;
	class RigidBody2D;

	class Physics2DSubsystem final : public ISubsystem, public IUpdatable, public IFixedUpdatable
	{
	private:
		Physics2DSettings m_settings;																								// Declared before m_defaultScene: the scene holds a pointer to it
		PhysicsScene2D m_defaultScene;																								// The PhysicsScene2D automatically created when Caledon starts

	public:
		static constexpr int AllLayers = Physics2DConstants::kAllLayers;															// Layer mask constant that includes all layers
		static constexpr int IgnoreRaycastLayer = Physics2DConstants::kIgnoreRaycastLayer;											// Layer mask constant for the default layer that ignores raycasts
		static constexpr int DefaultRaycastLayers = Physics2DConstants::kDefaultRaycastLayers;										// Layer mask constant that includes all layers participating in raycasts by default
		static constexpr std::string_view kName = "Physics2D";																		// The published name of this subsystem in EngineContext

	public:
		Physics2DSubsystem();																										// Constructor
		~Physics2DSubsystem() override = default;																					// Destructor
		Physics2DSubsystem(const Physics2DSubsystem&) = delete;																		// Prevent copy-construction
		Physics2DSubsystem& operator=(const Physics2DSubsystem&) = delete;															// Prevent copy-assignment
		Physics2DSubsystem(Physics2DSubsystem&&) = delete;																			// Prevent move-construction
		Physics2DSubsystem& operator=(Physics2DSubsystem&&) = delete;																// Prevent move-assignment

		std::string_view Name() const noexcept override;																			// Returns the name of the subsystem
		bool Initialize(EngineContext& context) override;																			// Prepares the Physics2DSubsystem for use
		void Shutdown() noexcept override;																							// Cleans up the Physics2DSubsystem and releases any resources
		void Update(float deltaTime) override;																						// Updates the Physics2DSubsystem and its default PhysicsScene2D if the simulation mode is set to Update
		void FixedUpdate(float fixedDeltaTime) override;																			// Updates the Physics2DSubsystem and its default PhysicsScene2D if the simulation mode is set to FixedUpdate

		/*--------------------------------------------------------------------------------------------------------------------
		| --- Settings: Read access to every physics setting. Changes go through SetSettings or a validated setter below --- |
		--------------------------------------------------------------------------------------------------------------------*/
		// (NOT YET) PhysicsJobOptions2D jobOptions: will live in Physics2DSettings once the job system exists

		const Physics2DSettings& Settings() const noexcept								{ return m_settings; }						// Returns the physics settings for the Physics2DSubsystem and its default PhysicsScene2D (read-only)
		void SetSettings(const Physics2DSettings& settings);																		// Sets the physics settings for the Physics2DSubsystem and its default PhysicsScene2D (write access)
		PhysicsScene2D& GetDefaultPhysicsScene() noexcept								{ return m_defaultScene; }					// Returns a reference to the default PhysicsScene2D automatically created when Caledon starts (read/write)
		const PhysicsScene2D& GetDefaultPhysicsScene() const noexcept					{ return m_defaultScene; }					// Returns a const reference to the default PhysicsScene2D automatically created when Caledon starts (read-only)

		void SetGravity(const Vector2f& gravity);																					// Sets the global gravity vector for the 2D physics simulation
		void SetPositionIterations(int iterations);																					// Sets the number of iterations of the physics solver when considering objects' positions
		void SetVelocityIterations(int iterations);																					// Sets the number of iterations of the physics solver when considering objects' velocities
		void SetMaxSubStepCount(int count);																							// Sets the maximum number of simulation sub-steps allowed per-frame when simulation sub-stepping is enabled
		void SetMinSubStepFPS(float fps);																							// Sets the minimum FPS allowed for a simulation step before sub-stepping will be used

		bool GetIgnoreCollision(const Collider2D* pCollider1, const Collider2D* pCollider2) const;									// Checks whether collision between two Collider2D are to be ignored
		void SetIgnoreCollision(Collider2D* pCollider1, Collider2D* pCollider2, bool ignore = true);								// Sets a collision between two Collider2D to be ignored (or not)
		bool GetIgnoreLayerCollision(int layer1, int layer2) const;																	// Checks whether collisions between the specified layers are ignored or not
		void SetIgnoreLayerCollision(int layer1, int layer2, bool ignore);															// Chooses whether to detect or ignore collisions between a specified pair of layers
		int GetLayerCollisionMask(int layer) const;																					// Gets the collision layer mask that indicates which layer(s) the specified layer can collide with
		void SetLayerCollisionMask(int layer, int layerMask);																		// Sets the collision layer mask that indicates which layer(s) the specified layer can collide with

		int GetContacts(const Collider2D* pCollider, const ContactFilter2D& contactFilter,											// Retrieves all of the ContactPoint2D with the given Collider2D, with the results filtered by the ContactFilter2D
			std::span<ContactPoint2D> contacts) const;

		int GetContactColliders(const Collider2D* pCollider, const ContactFilter2D& contactFilter,									// Retrieves all of the unique Collider2D in contact with the given Collider2D, with the results filtered by the ContactFilter2D
			std::vector<Collider2D*>& results) const;

		bool IsTouching(const Collider2D* pCollider1, const Collider2D* pCollider2) const;											// Returns whether the two Collider2D were in contact during the latest step
		bool IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const;									// Returns whether the given Collider2D was touching any other Collider2D that passes the ContactFilter2D during the latest step
		bool IsTouching(const ContactFilter2D& contactFilter) const;																// 
		bool IsTouchingLayers(const Collider2D* pCollider, LayerMask layerMask = AllLayers) const;									// Returns whether the given Collider2D is touching any other Collider2D on the specified LayerMask

		/*--------------------------------------------------------------------------------------------------------------
		| --- BoxCast: Returns all intersections of a casted Box against the Collider2D in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity) const;
		int BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance = Math::Infinity) const;

		/*-------------------------------------------------------------------------------------------------------------------
		| --- BoxCastAll: Returns all of the unique Collider2D intersected by the casted Box in this Physics2DSubsystem --- |
		-------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> BoxCastAll(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction,
			float distance = Math::Infinity, int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*----------------------------------------------------------------------------------------------------------------------
		| --- CapsuleCast: Returns all intersections of a casted Capsule against the Collider2D in this Physics2DSubsystem --- |
		----------------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity) const;
		int CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance = Math::Infinity) const;

		/*---------------------------------------------------------------------------------------------------------------------------
		| --- CapsuleCastAll: Returns all of the unique Collider2D intersected by the casted Capsule in this Physics2DSubsystem --- |
		---------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> CapsuleCastAll(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle,
			const Vector2f& direction, float distance = Math::Infinity, int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*--------------------------------------------------------------------------------------------------------------------
		| --- CircleCast: Returns all intersections of a casted Circle against the Collider2D in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity) const;
		int CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance = Math::Infinity) const;

		/*-------------------------------------------------------------------------------------------------------------------------
		| --- CircleCastAll: Returns all of the unique Collider2D intersected by the casted Circle in this Physics2DSubsystem --- |
		-------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> CircleCastAll(const Vector2f& origin, float radius, const Vector2f& direction, float distance = Math::Infinity,
			int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*--------------------------------------------------------------------------------------------------------------
		| --- Raycast: Returns all intersections of a casted Ray against the Collider2D in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity) const;
		int Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance = Math::Infinity) const;

		/*-------------------------------------------------------------------------------------------------------------------
		| --- RaycastAll: Returns all of the unique Collider2D intersected by the casted Ray in this Physics2DSubsystem --- |
		-------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> RaycastAll(const Vector2f& origin, const Vector2f& direction, float distance = Math::Infinity,
			int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*------------------------------------------------------------------------------------------------------------------------
		| --- Linecast: Returns all intersections of a casted Line segment against the Collider2D in this Physics2DSubsystem --- |
		------------------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter) const;
		int Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const;

		/*-----------------------------------------------------------------------------------------------------------------------------
		| --- LinecastAll: Returns all of the unique Collider2D intersected by the casted Line segment in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> LinecastAll(const Vector2f& start, const Vector2f& end, int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*-------------------------------------------------------------------------------------------------------------------------
		| --- OverlapArea: Returns all intersections of an overlapping Area against the Collider2D in this Physics2DSubsystem --- |
		-------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter) const;
		int OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*-----------------------------------------------------------------------------------------------------------------------------
		| --- OverlapAreaAll: Returns all of the unique Collider2D intersected by the overlapping Area in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapAreaAll(const Vector2f& pointA, const Vector2f& pointB, int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*-----------------------------------------------------------------------------------------------------------------------
		| --- OverlapBox: Returns all intersections of an overlapping Box against the Collider2D in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter) const;
		int OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*---------------------------------------------------------------------------------------------------------------------------
		| --- OverlapBoxAll: Returns all of the unique Collider2D intersected by the overlapping Box in this Physics2DSubsystem --- |
		---------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapBoxAll(const Vector2f& point, const Vector2f& size, float angle, int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*-------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Collider2D in this Physics2DSubsystem --- |
		-------------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter) const;
		int OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*-----------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCapsuleAll: Returns all of the unique Collider2D intersected by the overlapping Capsule in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCapsuleAll(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection,
			float angle, int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*-----------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Collider2D in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter) const;
		int OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*---------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCircleAll: Returns all of the unique Collider2D intersected by the overlapping Circle in this Physics2DSubsystem --- |
		---------------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCircleAll(const Vector2f& point, float radius, int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*-----------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCollider: Returns all intersections of an overlapping Collider2D against the Collider2D in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const;
		int OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*---------------------------------------------------------------------------------------------------------------------------
		| --- OverlapPoint: Returns all intersections of an overlapping Point against the Collider2D in this Physics2DSubsystem --- |
		---------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter) const;
		int OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const;

		/*-------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapPointAll: Returns all of the unique Collider2D intersected by the overlapping Point in this Physics2DSubsystem --- |
		-------------------------------------------------------------------------------------------------------------------------------*/
		std::vector<Collider2D*> OverlapPointAll(const Vector2f& point, int layerMask = DefaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity) const;

		/*---------------------------------------------------------------------------------------------------------------------------
		| --- GetRayIntersection: Returns the nearest intersection of a Ray2D against the Collider2D in this Physics2DSubsystem --- |
		---------------------------------------------------------------------------------------------------------------------------*/
		ColliderHit2D GetRayIntersection(const Ray2D& ray, float distance = Math::Infinity, int layerMask = DefaultRaycastLayers) const;

		/*-----------------------------------------------------------------------------------------------------------------------
		| --- GetRayIntersectionAll: Returns all of the unique Collider2D intersected by a Ray2D in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------------*/
		std::vector<ColliderHit2D> GetRayIntersectionAll(const Ray2D& ray, float distance = Math::Infinity, int layerMask = DefaultRaycastLayers) const;

		Vector2f ClosestPoint(const Vector2f& position, const Collider2D* pCollider) const;											// Returns a point on the perimeter of the given Collider2D that is closest to the specified position
		ColliderDistance2D Distance(const Collider2D* pColliderA, const Collider2D* pColliderB) const;								// Calculates the minimum distance between two Collider2D

		bool Simulate(float deltaTime, int simulatedLayers = AllLayers);															// Simulates physics in the default 2D physics scene
		void SyncTransforms();																										// Synchronizes transform positions with 2D physics bodies
	};
}