/*------------------------------
| File: ContactSolver2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/ShapeManifold2D.h"
#include "Utilities/Math/Vector2.h"

#include <span>

// Pure contact solver: makes bodies bounce, slide and rest on each other instead of passing through. It works on plain structs and knows
// nothing about colliders, bodies or scenes (the PhysicsScene2D copies its bodies into SolverBody2D, solves, and copies the result back)
//
// The method is "sequential impulses": every contact point gets an impulse (an instant push) that removes the velocity pushing the two bodies
// into each other. Fixing one contact disturbs the others, so all contacts are visited again and again (the iterations) until the pushes settle
// * Friction is solved before the normal push, because the normal push is the stronger constraint and should have the last word
// * The accumulated impulse of each point is clamped, not the impulse of each iteration: a normal impulse may never pull (>= 0) and friction can
//   never be larger than friction * normal impulse (Coulomb's law)
// * A separate position pass then nudges overlapping bodies apart, without adding velocity (so resting contacts don't gain energy)
//
// References (the structure follows Box2D 2.4's b2ContactSolver, the engine behind Unity's 2D physics)
// * Erin Catto, "Iterative Dynamics with Temporal Coherence" (GDC 2005) and "Fast and Simple Physics using Sequential Impulses" (GDC 2006), https://box2d.org/publications/
// * Box2D 2.4: src/dynamics/b2_contact_solver.cpp -> b2ContactSolver::InitializeVelocityConstraints, WarmStart, SolveVelocityConstraints, b2SolvePositionConstraints
//   (github.com/erincatto/box2d, v2.4.x; the repository layout was not verified from here)
// * Box2D constants: b2_linearSlop = 0.005, b2_baumgarte = 0.2, b2_maxLinearCorrection = 0.2, b2_velocityThreshold = 1.0 (b2_common.h / b2_settings.h)
//
// Units: world units, seconds and RADIANS (the RigidBody2D stores degrees, the scene converts)
// Conventions: the normal points from body A to body B. A contact point is described by two "anchors": the vectors from the center of mass of
// each body to the contact point at the time the constraint was built

namespace CaledonEngine
{
	struct SolverBody2D
	{
		Vector2f position{ };																					// World position of the center of mass
		float angle = 0.0f;																						// Rotation in radians
		Vector2f linearVelocity{ };																				// World units per second
		float angularVelocity = 0.0f;																			// Radians per second

		float inverseMass = 0.0f;																				// 1 / mass (0 = static or kinematic: it is never pushed)
		float inverseInertia = 0.0f;																			// 1 / inertia (0 = it never spins from a push, e.g. frozen rotation)
		Vector2f linearFactor{ 1.0f, 1.0f };																	// 1 = the body may move along that axis, 0 = frozen (position constraints)
	};

	struct ContactSolverSettings2D
	{
		float bounceThreshold = 1.0f;																			// Closing speeds below this don't bounce
		float baumgarte = 0.2f;																					// The fraction of the overlap that one position iteration removes
		float linearSlop = 0.005f;																				// Overlap that is tolerated so resting contacts don't flicker on and off
		float maxLinearCorrection = 0.2f;																		// The largest position correction of one iteration (prevents overshoot)
	};

	struct ContactImpulses2D
	{
		float normal[2]{ 0.0f, 0.0f };																			// Accumulated normal impulse at each point
		float tangent[2]{ 0.0f, 0.0f };																			// Accumulated friction impulse at each point
	};

	struct ContactPointConstraint2D
	{
		Vector2f anchorA{ };																					// Contact point minus center of mass of A, when built
		Vector2f anchorB{ };																					// Contact point minus center of mass of B, when built
		float separation = 0.0f;																				// Gap along the normal when built (negative = overlap)
		float normalMass = 0.0f;																				// 1 / (how hard it is to push along the normal): impulse per unit velocity
		float tangentMass = 0.0f;																				// The same along the surface
		float velocityBias = 0.0f;																				// The speed the normal velocity should be pushed to (the bounce)
		float normalImpulse = 0.0f;																				// Accumulated normal impulse (never negative)
		float tangentImpulse = 0.0f;																			// Accumulated friction impulse
	};

	struct ContactConstraint2D
	{
		int bodyA = -1;																							// Index of body A in the body array
		int bodyB = -1;																							// Index of body B in the body array
		Vector2f normal{ };																						// From A to B
		float friction = 0.0f;
		float restitution = 0.0f;
		float angleA = 0.0f;																					// Rotation of A when built (anchors are rotated by the change since then)
		float angleB = 0.0f;
		int pointCount = 0;
		ContactPointConstraint2D points[2];
	};

	namespace ContactSolver2D
	{
		ContactConstraint2D Build(const ShapeManifold2D& manifold, float friction, float restitution, int bodyA, int bodyB,						// Builds a contact constraint from a manifold, including effective masses, restitution bias and opitonal warm-start impulses
			std::span<const SolverBody2D> bodies, const ContactSolverSettings2D& settings, const ContactImpulses2D* warm = nullptr);

		void WarmStart(std::span<const ContactConstraint2D> constraints, std::span<SolverBody2D> bodies);										// Applies previously accumulated contact impulses to the bodies
		void SolveVelocity(std::span<ContactConstraint2D> constraints, std::span<SolverBody2D> bodies);											// Iteratively resolves relative velocities along contact normals and tangents
		bool SolvePosition(std::span<const ContactConstraint2D> constraints, std::span<SolverBody2D> bodies,									// Corrects positional overlap and returns whether the constraints have sufficiently converged
			const ContactSolverSettings2D& settings);
		
		ContactImpulses2D GetImpulses(const ContactConstraint2D& constraint) noexcept;															// Retrieves the accumulated normal and frition impulses from a contact constraint
	}
}
