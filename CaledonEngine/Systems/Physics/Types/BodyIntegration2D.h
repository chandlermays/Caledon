/*------------------------------
| File: BodyIntegration2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Math/Vector2.h"

// Pure motion math for one body, with no knowledge of RigidBody2D, scenes or colliders
//
// Integration is "semi-implicit Euler": the velocity is updated first (forces, gravity, damping, speed limits), and the NEW velocity is then used to move the body.
// This is the same scheme Box2D uses (the engine behind Unity's 2D physics) and it stays stable for games, unlike plain explicit Euler
//
// Units: world units, seconds, and degrees for angles (like the Transform and RigidBody2D)

namespace CaledonEngine
{
	struct BodyVelocityInput2D
	{
		Vector2f linearVelocity{ };																				// The current velocity of the center of mass
		float angularVelocity = 0.0f;																			// The current spin in degrees per second

		Vector2f acceleration{ };																				// The acceleration to add this step (gravity * gravityScale, from the scene)
		Vector2f force{ };																						// The force applied since the last step
		float torque = 0.0f;																					// The torque applied since the last step
		float inverseMass = 0.0f;																				// 1 / mass (0 means the body cannot be pushed by forces)
		float inverseInertia = 0.0f;																			// 1 / inertia (0 means the body cannot be spun by torque)

		float linearDamping = 0.0f;																				// How quickly the linear velocity fades out (per second)
		float angularDamping = 0.0f;																			// How quickly the spin fades out (per second)
		float maxTranslationSpeed = 0.0f;																		// The speed limit in world units per second (0 or less = no limit)
		float maxRotationSpeed = 0.0f;																			// The spin limit in degrees per second (0 or less = no limit)
	};

	namespace BodyIntegration2D
	{
		void IntegrateVelocity(const BodyVelocityInput2D& input, float deltaTime, Vector2f& linearVelocity, float& angularVelocity);		// Advances the velocities by one step: forces, gravity, damping, then the speed limits
		float ApplyDamping(float velocity, float damping, float deltaTime) noexcept;															// Returns velocity after damping for deltaTime
	}
}
