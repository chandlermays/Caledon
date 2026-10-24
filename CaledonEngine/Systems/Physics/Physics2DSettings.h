/*------------------------------
| File: Physics2DSettings.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Types/SimulationMode2D.h"
#include "Systems/Physics/LayerMask.h"
#include "Utilities/Math/Vector2.h"

#include <array>

// The global 2D physics settings. One instance lives inside the Physics2DSubsystem and is shared by reference with its PhysicsScene2D(s)

namespace CaledonEngine
{
	namespace Physics2DConstants
	{
		inline constexpr int kAllLayers = -1;												// Layer mask constant that includes all layers
		inline constexpr int kIgnoreRaycastLayer = 1 << 2;									// Layer mask constant for the default layer that ignores raycasts
		inline constexpr int kDefaultRaycastLayers = ~kIgnoreRaycastLayer;					// Layer mask constant that includes all layers participating in raycasts by default
		inline constexpr int kLayerCount = 32;												// The number of layers
	}

	struct Physics2DSettings
	{
		float angularSleepTolerance = 2.0f;													// A RigidBody2D cannot sleep if its angular velocity is above this tolerance threshold
		float baumgarteScale = 0.2f;														// The scale factor that controls how fast overlaps are resolved
		float baumgarteTOIScale = 0.75f;													// The scale factor that controls how fast TOI overlaps are resolved
		float bounceThreshold = 1.0f;														// Any collisions with a relative linear velocity below this threshold will be treated as inelastic so no bounce will occur
		bool callbacksOnDisable = true;														// Whether the appropriate OnCollisionExit2D or OnTriggerExit2D callbacks should be called when a Collider2D is disabled
		float contactThreshold = 0.0001f;													// A threshold below which a contact is automatically disabled
		float defaultContactThreshold = 0.01f;												// The default contact offset of the newly created Colliders
		Vector2f gravity{ 0.0f, -9.81f };													// Acceleration due to gravity
		float linearSleepTolerance = 0.01f;													// A RigidBody2D cannot sleep if its linear velocity is above this tolerance
		float maxAngularCorrection = 8.0f;													// The maximum angular position correction used when solving constraints. This helps to prevent overshoot
		float maxLinearCorrection = 0.2f;													// The maximum linear position correction used when solving constraints. This helps to prevent overshoot
		int maxPolygonShapeVertices = 8;													// The maximum number of vertices allowed per primitive polygon shape type (read only in Unity)
		float maxRotationSpeed = 360.0f;													// The maximum angular speed of a RigidBody2D in degrees per second. Increasing this can cause numerical problems
		int maxSubStepCount = 3;															// The maximum number of simulation sub-steps allowed per-frame when simulation sub-stepping is enabled
		float maxTranslationSpeed = 100.0f;													// The maximum linear speed of a RigidBody2D in world-units per second. Increasing this can cause numerical problems
		float minSubStepFPS = 30.0f;														// The minimum FPS allowed for a simulation step before sub-stepping will be used
		int positionIterations = 3;															// The number of iterations of the physics solver when considering objects' positions
		bool queriesHitTriggers = true;														// Do the layer-mask based queries (the *All functions) detect Colliders configured as triggers
		bool queriesStartInColliders = true;												// Set the casts that start inside Colliders to detect or not detect those Colliders
		bool reuseCollisionCallbacks = true;												// Whether a single Collision2D instance is reused for all collision callbacks
		LayerMask simulationLayers{ Physics2DConstants::kAllLayers };						// The RigidBody2D and Collider2D layers to simulate
		SimulationMode2D simulationMode = SimulationMode2D::FixedUpdate;					// Controls when the 2D physics simulation executes
		float timeToSleep = 0.5f;															// The time in seconds that a RigidBody2D must be still before it will go to sleep
		bool useSubStepContacts = false;													// Whether to calculate contacts for all simulation sub-steps or only the first sub-step
		bool useSubStepping = false;														// Whether to use simulation sub-stepping during a simulation step
		int velocityIterations = 8;															// The number of iterations of the physics solver when considering objects' velocities

		std::array<int, Physics2DConstants::kLayerCount> layerCollisionMatrix = []()		// For each layer, the mask of layers it collides with (symmetric). Everything collides by default
			{
				std::array<int, Physics2DConstants::kLayerCount> matrix{ };
				matrix.fill(-1);
				return matrix;
			}();
	};
}