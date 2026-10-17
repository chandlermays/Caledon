/*------------------------------
| File: PhysicsMaterial2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "PhysicsMaterialCombine2D.h"

// Defines the surface properties of a Collider2D.
// When two Collider2D come into contact, the physics system uses both
// friction and bounciness if it needs to calculate a collision response.
//
// Materials are plain shared data: a Collider2D (or RigidBody2D) holds a std::shared_ptr to one, and a collider with no
// material uses the PhysicsSubsystem's default. There is deliberately no global default here: a static would be a different
// object in every exe/dll that links the engine library.

namespace CaledonEngine
{
	class PhysicsMaterial2D
	{
	private:
		float m_friction;															// Coefficient of friction; controls how much a collision response reduces velocity. 0 = no friction, higher values produce increasing friction
		float m_bounciness;															// Coefficient of restitution; controls how "elastic" a collision response is. 0 = no bounce, 1 = perfectly elastic

		PhysicsMaterialCombine2D m_frictionCombine;									// Determines how the effective friction is calculated when two Collider2D come into contact. Defaults to Mean, which allows either Collider2D to reduce friction toward zero (e.g. anything slides on ice)
		PhysicsMaterialCombine2D m_bounceCombine;									// Determines how the effective bounciness is calculated when two Collider2D come into contact. Defaults to Maximum, which allows either Collider2D to maximize bounciness (e.g. a very bouncy ball always remains very bouncy)

	public:
		PhysicsMaterial2D();														// Constructor: default friction/bounciness with default combine modes
		PhysicsMaterial2D(float friction, float bounciness);						// Constructor: constructs a PhysicsMaterial2D with the given friction and bounciness, using default combine modes
		~PhysicsMaterial2D() = default;												// Destructor

		float GetFriction() const;													// Returns the coefficient of friction
		void SetFriction(float friction);											// Sets the coefficient of friction

		float GetBounciness() const;												// Returns the coefficient of restitution
		void SetBounciness(float bounciness);										// Sets the coefficient of restitution

		PhysicsMaterialCombine2D GetFrictionCombine() const;						// Returns the combine mode used when calculating effective friction
		void SetFrictionCombine(PhysicsMaterialCombine2D combine);					// Sets the combine mode used when calculating effective friction

		PhysicsMaterialCombine2D GetBounceCombine() const;							// Returns the combine mode used when calculating effective bounciness
		void SetBounceCombine(PhysicsMaterialCombine2D combine);					// Sets the combine mode used when calculating effective bounciness

		// Returns the effective friction or bounciness value used in the collision response between two Collider2D.
		// valueA/valueB: the friction or bounciness value used by each Collider2D's PhysicsMaterial2D
		// combineA/combineB: the combine mode used by each Collider2D's PhysicsMaterial2D
		static float GetCombinedValues(float valueA, float valueB, PhysicsMaterialCombine2D combineA, PhysicsMaterialCombine2D combineB);
	};
}