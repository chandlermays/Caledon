/*------------------------------
| File: ContactPoint2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Math/Vector2.h"

// Details about a specific point of contact involved in a 2D physics collision
// A contact point described a point of intersection between two Collider2D
//
// 'collider', 'otherCollider', 'rigidbody' and 'otherRigidbody' are non-owning pointers into the Scene, valid for the
// duration of the callback that delivered this contact. Don't keep a ContactPoint2D across frames.

namespace CaledonEngine
{
	class Collider2D;
	class RigidBody2D;

	struct ContactPoint2D
	{
		Collider2D* pCollider = nullptr;													// The incoming Collider2D involved in this ContactPoint2D
		Collider2D* pOtherCollider = nullptr;												// The other Collider2D involved in this ContactPoint2D

		RigidBody2D* pRigidBody = nullptr;													// The incoming RigidBody2D involved in this ContactPoint2D
		RigidBody2D* pOtherRigidBody = nullptr;												// The other RigidBody2D involved in this ContactPoint2D

		Vector2f normal{};																	// The normal vector that is perpendicular to the surface or the incoming Collider2D at the contact point
		Vector2f point{};																	// The point of contact between two Collider2D in world space
		Vector2f relativeVelocity{};														// The difference in velocity between two moving and rotating Collider2D at the contact point

		float bounciness = 0.0f;															// The effective bounciness used for this ContactPoint2D
		float friction = 0.0f;																// The effective friction used for this ContactPoint2D
		float normalImpulse = 0.0f;															// The impulse applied at this ContactPoint2D along the normal vector
		float separation = 0.0f;															// The distance between the two Collider2D at this ContactPoint2D
		float tangentImpulse = 0.0f;														// The impulse applied at this ContactPoint2D along the tangent vector (perpendicular to the normal)

		bool enabled = true;																// Indicates whether this ContactPoint2D is enabled (can be disabled in callbacks to ignore the contact)
	};
}