/*------------------------------
| File: Physics2DSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IFixedUpdatable.h"

#include <string_view>

// Provides utilities and global settings to manage and simulate 2D physics interactions

namespace CaledonEngine
{
	class Physics2DSubsystem : public ISubsystem, public IFixedUpdatable
	{
//	static properties:
//		static int AllLayers												// Layer mask constant that includes all layers
//		static float angularSleepTolerance									// A Rigidbody cannot sleep if its angular velocity is above this tolerance threshold
//		static float baumgarteScale											// The scale factor that controls how fast overlaps are resolved
//		static float baumgarteTOIScale										// The scale factor that controls how fast TOI overlaps are resolved
//		static float bounceThreshold										// Any collisions with a relative linear velocity below this threshold will be treated as inelastic so no bounce will occur
//		static bool callbacksOnDisable										// Use this to control whether or not the appropriate OnCollisionExit2D or OnTriggerExit2D callbacks should be called when a Collider2D is disabled
//		static float contactThreshold										// A threshold below which a contact is automatically disabled
//		static float defaultContactThreshold								// The default contact offset of the newly created Colliders
//		static PhysicsScene2D defaultPhysicsScene							// The PhysicsScene2D automatically created when Unity starts
//		static int DefaultRaycastLayers										// Layer mask constant that includes all layers participating in raycasts by default
//		static Vector2 gravity												// Acceleration due to gravity
//		static int IgnoreRaycastLayer										// Layer mask constant for the default layer that ignores raycasts
//		(NOT YET) static PhysicsJobOptions2D jobOptions						// A set of options that control how physics operates when using the job system to multithread the physics simulation
//		static float linearSleepTolerance									// A rigid-body cannot sleep if its linear velocity is above this tolerance
//		static float maxAngularCorrection									// The maximum angular position correction used when solving constraints. This helps to prevent overshoot
//		static float maxLinearCorrection									// The maximum linear position correction used when solving constraints. This helps to prevent overshoot
//		static int MaxPolygonShapeVertices									// The maximum number of vertices allowed per primitive polygon shape type (PhysicsShapeType2D.Polygon). (Read Only
//		static float maxRotationSpeed										// The maximum angular speed of a rigid-body per physics update. Increasing this can cause numerical problems
//		static int maxSubStepCount											// The maximum number of simulation sub-steps allowed per-frame when simulation sub-stepping is enabled
//		static float maxTranslationSpeed									// The maximum linear speed of a rigid-body per physics update. Increasing this can cause numerical problems
//		static float minSubStepFPS											// The minimum FPS allowed for a simulation step before sub-stepping will be used
//		static int positionIterations										// The number of iterations of the physics solver when considering objects' positions
//		static bool queriesHitTriggers										// Do raycasts detect Colliders configured as triggers
//		static bool queriesStartInColliders									// Set the raycasts or linecasts that start inside Colliders to detect or not detect those Colliders
//		static bool reuseCollisionCallbacks									// Determines whether the garbage collector should reuse only a single instance of a Collision2D type for all collision callbacks
//		static LayerMask simulationLayers									// The Rigidbody2D and Collider2D layers to simulate
//		static SimulationMode2D simulationMode								// Controls when Unity executes the 2D physics simulation
//		static float timeToSleep											// The time in seconds that a rigid-body must be still before it will go to sleep
//		static bool useSubStepContacts										// Whether to calculate contacts for all simulation sub-steps or only the first sub-step
//		static bool useSubStepping											// Whether to use simulation sub-stepping during a simulation step
//		static int velocityIterations										// The number of iterations of the physics solver when considering objects' velocities
//
//  static methods:
//		
//		static NativeArray<ColliderHit2D> BoxCast(Vec2 origin, Vec2 size,							// Casts a box against the Colliders in the Scene and returns all Colliders that are in contact with it
//			float angle, Vec2 direction, ContactFilter2D contactFilter,
//			float distance = Math::INFINITY, Allocator allocator)
// 
//		static ColliderHit2D[] BoxCastAll(Vec2 origin, Vec2 size, float angle,						// Casts a box against Colliders in the Scene, returning all Colliders that contact with it
//			Vec2 direction, float distance = Math::INFINITY,
//			int layerMask = DefaultRaycastLayers, float minDepth = -Math::INFINITY,
//			float maxDepth = Math::INFINITY)
// 
//		static NativeArray<ColliderHit2D> CapsuleCast(Vec2 origin, Vec2 size,						// Casts a capsule against the Colliders in the Scene and returns all Colliders that are in contact with it
//			CapsuleDirection2D capsuleDirection, float angle, Vec2 direction,
//			ContactFilter2D contactFilter, float distance = Math::INFINITY,
//			Allocator allocator)
// 
//		static ColliderHit2D[] CapsuleCastAll(Vec2 origin, Vec2 size,								// Casts a capsule against Colliders in the Scene, returning all Colliders that contact with it
//			CapsuleDirection2D capsuleDirection, float angle, Vec2 direction,
//			float distance = Math::INFINITY, int layerMask = DefaultRaycastLayers,
//			float minDepth = -Math::INFINITY, float maxDepth = Math::INFINITY)
// 
//		static NativeArray<ColliderHit2D> CircleCast(Vec2 origin, float radius,						// Casts a circle against Colliders in the Scene, returning all Colliders that contact with it
//			Vec2 direction, ContactFilter2D contactFilter,
//			float distance = Math::INFINITY, Allocator allocator)
// 
//		static ColliderHit2D[] CircleCastAll(Vec2 origin, float radius, Vec2 direction,				// Casts a circle against Colliders in the Scene, returning all Colliders that contact with it
//			float distance = Math::INFINITY, int layerMask = DefaultRaycastLayers,
//			float minDepth = -Math::INFINITY, float maxDepth = Math::INFINITY)
// 		   
//		static Vector2 ClosestPoint(Vector2 position, Collider2D collider)							// Returns a point on the perimeter of the Collider that is closest to the specified position
//		static ColliderDistance2D Distance(Collider2D colliderA, Collider2D colliderB)				// Calculates the minimum distance between two Colliders
// 
//		static ColliderArray2D GetContactColliders(Collider2D collider,								// Retrieves all colliders in contact with this Collider, with the results filtered by the contactFilter
//			ContactFilter2D contactFilter, Allocator allocator)
// 
//		static NativeArray<ContactPoint2D> GetContacts(Collider2D collider,							// Retrieves all contact points in contact with the Collider, with the results filtered by the contactFilter2D
//			ContactFilter2D contactFilter, Allocator allocator)
// 
//		static bool GetIgnoreCollision(Collider2D collider1, Collider2D collider2)					// Checks whether the collision detection system will ignore all collisions / triggers between collider1 and collider2 or not
//		static bool GetIgnoreLayerCollision(int layer1, int layer2)									// Checks whether collisions between the specified layers be ignored or not
//		static int GetLayerCollisionMask(int layer)													// Get the collision layer mask that indicates which layer(s) the specified layer can collide with
// 
//		static NativeArray<ColliderHit2D> GetRayIntersection(Ray ray, float distance,				// Cast a 3D ray against the 2D Colliders in the Scene
//			int layerMask = Physics2D.DefaultRaycastLayers, Allocator allocator)
// 
//		static ColliderHit2D[] GetRayIntersectionAll(Ray ray, float distance,						// Cast a 3D ray against the 2D Colliders in the Scene
//			int layerMask = DefaultRaycastLayers)
// 
//		static void IgnoreCollision(Collider2D collider1, Collider2D collider2,						// Makes the collision detection system ignore all collisions / triggers between collider1 and collider2
//			bool ignore = true)
// 
//		static void IgnoreLayerCollision(int layer1, int layer2, bool ignore)						// Choose whether to detect or ignore collisions between a specified pair of layers
//		static bool IsTouching(Collider2D collider1, Collider2D collider2)							// Checks whether the passed Colliders are in contact or not
// 
//		static bool IsTouchingLayers(Collider2D collider,											// Checks whether the Collider is touching any Colliders on the specified layerMask or not
//			int layerMaskk = Physics2D.AllLayers)
// 
//		static NativeArray<ColliderHit2D> Linecast(Vec2 start, Vec2 end,							// Casts a line segment against Colliders in the Scene with results filtered by ContactFilter2D
//			ContactFilter2D contactFilter, Allocator allocator)
// 
//		static ColliderHit2D[] LinecastAll(Vec2 start, Vec2 end,									// Casts a line against Colliders in the Scene
//			int layerMask = DefaultRaycastLayers,
//			float minDepth = Math::INFINITY, float maxDepth = Math::INFINITY)
// 
//		static ColliderArray2D OverlapArea(Vec2 pointA, Vec2 pointB,								// Finds Colliders that intersect a rectangular area
//			ContactFilter2D contactFilter, Allocator allocator)
// 
//		static Collider2D[] OverlapAreaAll(Vec2 pointA, Vec2 pointB									// Get a list of all Colliders that fall within a rectangular area
// 
//		static ColliderArray2D[] OverlapBox(Vec2 point, Vec2 size, float angle,						// Checks a box against Colliders in the scene, returning all intersections
//			ContactFilter2D contactFilter, Allocator allocator)
// 
//		static Collider2D[] OverlapBoxAll(Vec2 point, Vec2 size, float angle,						// Get a list of all Colliders that fall within a box area
//			int layerMask = DefaultRaycastLayers,
//			float minDepth = Math::INFINITY, float maxDepth = Math::INFINITY)
// 
//		static ColliderArray2D OverlapCapsule(Vec2 point, Vec2 size,								// Checks a capsule against Colliders in the scene, returning all intersections
//			CapsuleDirection2D direction, float angle,
//			ContactFilter2D contactFilter, Allocator allocator)
// 
//		static Collider2D[] OverlapCapsuleAll(Vec2 point, Vec2 size,								// Get a list of all Colliders that fall within a capsule area
//			CapsuleDirection2D direction, float angle,
//			int layerMask = DefaultRaycastLayers,
//			float minDepth = Math::INFINITY, float maxDepth = Math::INFINITY)
// 
//		static ColliderArray2D OverlapCircle(Vec2 point, float radius,								// Checks a circle against Colliders in the PhysicsScene2D, returning all intersections
//			ContactFilter2D contactFilter, Allocator allocator)
// 
//		static Collider2D[] OverlapCircleAll(Vec2 point, float radius,								// Get a list of all Colliders that fall within a circular area
//			int layerMask = DefaultRaycastLayer,
//			float minDepth = Math::INFINITY, float maxDepth = Math::INFINITY)
// 
//		static ColliderArray2D OverlapCollider(Collider2D collider,									// Gets a list of all Colliders that overlap the given Collider
//			ContactFilter2D contactFilter, Allocator allocator)
// 
//		static ColliderArray2D OverlapPoint(Vec2 point,												// Checks a point against Colliders in the scene, returning all intersections
//			ContactFilter contactFilter, Allocator allocator)
// 
//		static Collider2D[] OverlapPointAll(Vec2 point,												// Get a list of all Colliders that overlap a point in space
//			int layerMask = DefaultRaycastLayers,
//			float minDepth = Math::INFINITY, float maxDepth = Math::INFINITY)
// 
//		static NativeArray<ColliderHit2D> Raycast(Vec2 origin, Vec2 direction,						// Casts a ray against Colliders in the Scene
//			ContactFilter2D contactFilter, float distance = Math::INFINITY,
//			Allocator allocator)
// 
//		static ColliderHit2D[] RaycastAll(Vec2 origin, Vec2 direction,								// Casts a ray against Colliders in the Scene, returning all Colliders that contact with it
//			float distance = Math::INFINITY, int layerMask = DefaultRaycastLayers,
//			float minDepth = Math::INFINITY, float maxDepth = Math::INFINITY)
// 
//		static void SetLayerCollisionMask(int layer, int layerMask)									// Set the collision layer mask that indicates which layer(s) the specified layer can collide with
//		static bool Simulate(float deltaTime, int simulatedLayers = Physics2D.AllLayers)			// Simulate physics in the default physics scene
//		static void SyncTransforms																	// Synchronizes



	public:
		static constexpr std::string_view kName = "Physics2D";

		Physics2DSubsystem();																		// Constructor
		~Physics2DSubsystem() override = default;													// Destructor
		Physics2DSubsystem(const Physics2DSubsystem&) = delete;										// Prevent copy-construction
		Physics2DSubsystem& operator=(const Physics2DSubsystem&) = delete;							// Prevent copy-assignment
		Physics2DSubsystem(Physics2DSubsystem&&) = delete;											// Prevent move-construction
		Physics2DSubsystem& operator=(Physics2DSubsystem&&) = delete;								// Prevent move-assignment

		std::string_view Name() const noexcept override;
		bool Initialize(EngineContext& context) override;
		void Shutdown() noexcept override;

		void FixedUpdate(float fixedDeltaTime) override;
	};
}