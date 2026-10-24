/*------------------------------
| File: Physics2DSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IFixedUpdatable.h"
#include "Systems/Physics/Events/ContactPoint2D.h"
#include "Systems/Physics/Queries/ColliderHit2D.h"
#include "Systems/Physics/Queries/ContactFilter2D.h"
#include "Systems/Physics/Types/CapsuleDirection2D.h"
#include "Systems/Physics/Types/ColliderDistance2D.h"
#include "Systems/Physics/Types/SimulationMode2D.h"
#include "Systems/Physics/Physics2DSettings.h"
#include "Systems/Physics/PhysicsScene2D.h"
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

	class Physics2DSubsystem final : public ISubsystem, public IFixedUpdatable
	{
	private:
		static Physics2DSettings m_settings;
		static PhysicsScene2D s_defaultPhysicsScene;																					// The PhysicsScene2D automatically created when Caledon starts
		

	public:
		static constexpr int AllLayers = Physics2DConstants::kAllLayers;																// Layer mask constant that includes all layers
		static float s_angularSleepTolerance;																							// A Rigidbody cannot sleep if its angular velocity is above this tolerance threshold
		static float s_baumgarteScale;																									// The scale factor that controls how fast overlaps are resolved
		static float s_baumgarteTOIScale;																								// The scale factor that controls how fast TOI overlaps are resolved
		static float s_bounceThreshold;																									// Any collisions with a relative linear velocity below this threshold will be treated as inelastic so no bounce will occur
		static bool s_callbacksOnDisable;																								// Use this to control whether or not the appropriate OnCollisionExit2D or OnTriggerExit2D callbacks should be called when a Collider2D is disabled
		static float s_contactThreshold;																								// A threshold below which a contact is automatically disabled
		static float s_defaultContactThreshold;																							// The default contact offset of the newly created Colliders
		static int s_defaultRaycastLayers;																								// Layer mask constant that includes all layers participating in raycasts by default
		static Vector2f gravity;																										// Acceleration due to gravity
		static int IgnoreRaycastLayer;																									// Layer mask constant for the default layer that ignores raycasts
		// (NOT YET) static PhysicsJobOptions2D jobOptions;																			// A set of options that control how physics operates when using the job system to multithread the physics simulation
		static float s_linearSleepTolerance;																							// A rigid-body cannot sleep if its linear velocity is above this tolerance
		static float s_maxAngularCorrection;																							// The maximum angular position correction used when solving constraints. This helps to prevent overshoot
		static float s_maxLinearCorrection;																								// The maximum linear position correction used when solving constraints. This helps to prevent overshoot
		static int s_maxPolygonShapeVertices;																							// The maximum number of vertices allowed per primitive polygon shape type (PhysicsShapeType2D.Polygon). (Read Only
		static float s_maxRotationSpeed;																								// The maximum angular speed of a rigid-body per physics update. Increasing this can cause numerical problems
		static int s_maxSubStepCount;																									// The maximum number of simulation sub-steps allowed per-frame when simulation sub-stepping is enabled
		static float s_maxTranslationSpeed;																								// The maximum linear speed of a rigid-body per physics update. Increasing this can cause numerical problems
		static float s_minSubStepFPS;																									// The minimum FPS allowed for a simulation step before sub-stepping will be used
		static int s_positionIterations;																								// The number of iterations of the physics solver when considering objects' positions
		static bool s_queriesHitTriggers;																								// Do raycasts detect Colliders configured as triggers
		static bool s_queriesStartInColliders;																							// Set the raycasts or linecasts that start inside Colliders to detect or not detect those Colliders
		static bool s_reuseCollisionCallbacks;																							// Determines whether the garbage collector should reuse only a single instance of a Collision2D type for all collision callbacks
		static LayerMask s_simulationLayers;																							// The Rigidbody2D and Collider2D layers to simulate
		static SimulationMode2D s_simulationMode;																						// Controls when Unity executes the 2D physics simulation
		static float s_timeToSleep;																										// The time in seconds that a rigid-body must be still before it will go to sleep
		static bool s_useSubStepContacts;																								// Whether to calculate contacts for all simulation sub-steps or only the first sub-step
		static bool s_useSubStepping;																									// Whether to use simulation sub-stepping during a simulation step
		static int s_velocityIterations;																								// The number of iterations of the physics solver when considering objects' velocities

	public:
		Physics2DSubsystem();																											// Constructor
		~Physics2DSubsystem() override = default;																						// Destructor
		Physics2DSubsystem(const Physics2DSubsystem&) = delete;																			// Prevent copy-construction
		Physics2DSubsystem& operator=(const Physics2DSubsystem&) = delete;																// Prevent copy-assignment
		Physics2DSubsystem(Physics2DSubsystem&&) = delete;																				// Prevent move-construction
		Physics2DSubsystem& operator=(Physics2DSubsystem&&) = delete;																	// Prevent move-assignment

		static constexpr std::string_view kName = "Physics2D";																			// The published name of this subsystem in EngineContext

		std::string_view Name() const noexcept override;																				// Returns the name of this subsystem
		bool Initialize(EngineContext& context) override;																				// Prepares the Physics2DSubsystem for use
		void Shutdown() noexcept override;																								// 
		void FixedUpdate(float fixedDeltaTime) override;																				//

		/*-------------------------------------------------------------------------------------------------------------
		| --- BoxCast: Returns all intersections of a casted Box against the Colliders in this Physics2DSubsystem --- |
		-------------------------------------------------------------------------------------------------------------*/
		static std::vector<ColliderHit2D> BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity);
		static int BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results);

		/*------------------------------------------------------------------------------------------------------------------
		| --- BoxCastAll: Returns all of the unique Colliders intersected by the casted Box in this Physics2DSubsystem --- |
		------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> BoxCastAll(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity);

		/*--------------------------------------------------------------------------------------------------------------------
		| --- CapsuleCast: Returns all intersections of a casted Capsule against the Colliders in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------------*/
		static std::vector<ColliderHit2D> CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection,
			float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity);
		static int CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction,
			float distance, ContactFilter2D& contactFilter, std::span<ColliderHit2D> results);

		/*--------------------------------------------------------------------------------------------------------------------------
		| --- CapsuleCastAll: Returns all of the unique Colliders intersected by the casted Capsule in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> CapsuleCastAll(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle,
			const Vector2f& direction, float distance = Math::Infinity, int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*------------------------------------------------------------------------------------------------------------------
		| --- CircleCast: Returns all intersections of a casted Cicle against the Colliders in this Physics2DSubsystem --- |
		------------------------------------------------------------------------------------------------------------------*/
		static std::vector<ColliderHit2D> CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance = Math::Infinity);
		static int CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results);

		/*------------------------------------------------------------------------------------------------------------------------
		| --- CircleCastAll: Returns all of the unique Colliders intersected by the casted Circle in this Physics2DSubsystem --- |
		------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> CircleCastAll(const Vector2f& origin, float radius, const Vector2f& direction, float distance = Math::Infinity,
			int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*-------------------------------------------------------------------------------------------------------------
		| --- Raycast: Returns all intersections of a casted Ray against the Colliders in this Physics2DSubsystem --- |
		-------------------------------------------------------------------------------------------------------------*/
		static std::vector<ColliderHit2D> Raycast(const Vector2f& origin, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter);
		static int Raycast(const Vector2f& origin, const Vector2f& direction, float distance, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results);

		/*------------------------------------------------------------------------------------------------------------------
		| --- RaycastAll: Returns all of the unique Colliders intersected by the casted Ray in this Physics2DSubsystem --- |
		------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> RaycastAll(const Vector2f& origin, const Vector2f& direction, float distance = Math::Infinity,
			int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*-----------------------------------------------------------------------------------------------------------------------
		| --- Linecast: Returns all intersections of a casted Line segment against the Colliders in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------------*/
		static std::vector<ColliderHit2D> Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter);
		static int Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results);

		/*----------------------------------------------------------------------------------------------------------------------------
		| --- LinecastAll: Returns all of the unique Colliders intersected by the casted Line segment in this Physics2DSubsystem --- |
		----------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> LinecastAll(const Vector2f& start, const Vector2f& end, int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*------------------------------------------------------------------------------------------------------------------------
		| --- OverlapArea: Returns all intersections of an overlapping Area against the Colliders in this Physics2DSubsystem --- |
		------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter);
		static int OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results);

		/*----------------------------------------------------------------------------------------------------------------------------
		| --- OverlapAreaAll: Returns all of the unique Colliders intersected by the overlapping Area in this Physics2DSubsystem --- |
		----------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapAreaAll(const Vector2f& pointA, const Vector2f& pointB, int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*----------------------------------------------------------------------------------------------------------------------
		| --- OverlapBox: Returns all intersections of an overlapping Box against the Colliders in this Physics2DSubsystem --- |
		----------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter);
		static int OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results);

		/*--------------------------------------------------------------------------------------------------------------------------
		| --- OverlapBoxAll: Returns all of the unique Colliders intersected by the overlapping Box in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapBoxAll(const Vector2f& point, const Vector2f& size, float angle,	int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Colliders in this Physics2DSubsystem --- |
		------------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDiretion, float angle, const ContactFilter2D& contactFilter);
		static int OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results);

		/*----------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCapsuleAll: Returns all of the unique Colliders intersected by the overlapping Capsule in this Physics2DSubsystem --- |
		----------------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapCapsuleAll(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection,
			float angle, int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*----------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Colliders in this Physics2DSubsystem --- |
		----------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter);
		static int OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results);

		/*--------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCircleAll: Returns all of the unique Colliders intersected by the overlapping Circle in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapCircleAll(const Vector2f& point, float radius, int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*--------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapCollider: Returns all intersections of an overlapping Collider against the Colliders in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter);
		static int OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results);

		/*--------------------------------------------------------------------------------------------------------------------------
		| --- OverlapPoint: Returns all intersections of an overlapping Point against the Colliders in this Physics2DSubsystem --- |
		--------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter);
		static int OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results);

		/*------------------------------------------------------------------------------------------------------------------------------
		| --- OverlapPointAll: Returns all of the unique Colliders intersected by the overlapping Point in this Physics2DSubsystem --- |
		------------------------------------------------------------------------------------------------------------------------------*/
		static std::vector<Collider2D*> OverlapPointAll(const Vector2f& point, int layerMask = s_defaultRaycastLayers, float minDepth = -Math::Infinity, float maxDepth = Math::Infinity);

		/*-----------------------------------------------------------------------------------------------------------------
		| --- GetRayIntersection: Returns all intersections of a ray against the Colliders in this Physics2DSubsystem --- |
		-----------------------------------------------------------------------------------------------------------------*/
		static ColliderHit2D GetRayIntersection(const Ray2D& ray, float distance = Math::Infinity, int layerMask = s_defaultRaycastLayers);
		static std::vector<ColliderHit2D> GetRayIntersectionAll(const Ray2D& ray, float distance = Math::Infinity, int layerMask = s_defaultRaycastLayers);


		static Vector2f ClosestPoint(const Vector2f& position, const Collider2D* pCollider);											// Returns a point on the perimeter of the Collider that is closest to the specified position
		static ColliderDistance2D Distance(const Collider2D* pColliderA, const Collider2D* pColliderB);									// Calculates the minimum distance between two Colliders

		static int GetContactColliders(const Collider2D* pCollider, const ContactFilter2D& contactFilter,								// Retrieves all colliders in contact with this Collider, filtered by the contactFilter
			std::vector<Collider2D*>& results);
		static int GetContacts(const Collider2D* pCollider, const ContactFilter2D& contactFilter,										// Retrieves all contact points for the Collider, filtered by the contactFilter
			std::span<ContactPoint2D> contacts);

		static bool GetIgnoreCollision(const Collider2D* pCollider1, const Collider2D* pCollider2);										// Checks whether the collision detection system will ignore all collisions/triggers between collider1 and collider2
		static bool GetIgnoreLayerCollision(int layer1, int layer2);																	// Checks whether collisions between the specified layers are ignored or not
		static int GetLayerCollisionMask(int layer);																					// Gets the collision layer mask that indicates which layer(s) the specified layer can collide with

		static void IgnoreCollision(Collider2D* pCollider1, Collider2D* pCollider2, bool ignore = true);								// Makes the collision detection system ignore all collisions/triggers between collider1 and collider2
		static void IgnoreLayerCollision(int layer1, int layer2, bool ignore);															// Chooses whether to detect or ignore collisions between a specified pair of layers
		static bool IsTouching(const Collider2D* pCollider1, const Collider2D* pCollider2);												// Checks whether the passed Colliders are in contact or not

		static bool IsTouchingLayers(const Collider2D* pCollider, LayerMask layerMask = LayerMask{ -1 });								// Checks whether the Collider is touching any Colliders on the specified layerMask
			
		static void SetLayerCollisionMask(int layer, int layerMask);																	// Sets the collision layer mask that indicates which layer(s) the specified layer can collide with
		static bool Simulate(float deltaTime, int simulatedLayers = -1);																// Simulates physics in the default physics scene
		static void SyncTransforms();																									// Synchronizes transform positions with physics bodies		
	};
}