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
		Vector2f centroid{ };																// 
		Collider2D* pCollider = nullptr;													// 
		float distance = 0.0f;																// 
		float fraction = 0.0f;																// 
		Vector2f normal{ };																	// 
		Vector2f point{ };																	// 
		RigidBody2D* pRigidBody = nullptr;													// 

		bool IsValid() const noexcept { return pCollider != nullptr; }						//
		explicit operator bool() const noexcept { return IsValid(); }						// 

		Transform* GetTransform() const noexcept;											// 
	};
}