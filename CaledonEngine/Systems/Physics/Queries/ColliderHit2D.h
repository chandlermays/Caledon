/*------------------------------
| File: ColliderHit2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Math/Vector2.h"

// Stores the results of a 2D physics query, including the detected Collider2D.
// Always check for a valid result before accessing hit information.
// ColliderHit2D is a short-lived snapshot; its non-owning collider and rigidbody pointers
// may become invalid if the referenced objects are destroyed, so don't retain hits across frames.

namespace CaledonEngine
{
	class Collider2D;
	class RigidBody2D;
	class Transform;

	struct ColliderHit2D
	{
		Vector2f centroid{ };																		// The world-space center of the 2D physics query shape when it intersects
		Collider2D* pCollider = nullptr;															// The Collider2D detected by the 2D physics query
		float distance = 0.0f;																		// The distance the 2D physics query traversed before it detected a Collider2D
		float fraction = 0.0f;																		// The fraction of the distance specified to the 2D physics query before it detected a Collider2D
		Vector2f normal{ };																			// The normal vector that is perpendicular to the surface of the detected Collider2D at the point of intersection
		Vector2f point{ };																			// The point of intersection between the 2D physics query shape and the detected Collider2D in world space
		RigidBody2D* pRigidBody = nullptr;															// The RigidBody2D that the Collider2D detected by the 2D physics query is attached to

		bool IsValid() const noexcept					{ return pCollider != nullptr; }			// Returns whether the ColliderHit2D contains valid hit information
		explicit operator bool() const noexcept			{ return IsValid(); }						// Implicit operator to check whether the ColliderHit2D contains valid hit information

		Transform* GetTransform() const noexcept;													// Returns the Transform on the GameObject that the Collider2D is attached to
	};
}