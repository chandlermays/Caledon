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
		Collider2D* pCollider = nullptr;													// 
		Collider2D* pOtherCollider = nullptr;												// 

		RigidBody2D* pRigidBody = nullptr;													// 
		RigidBody2D* pOtherRigidBody = nullptr;												// 

		Vector2f normal{};																	// 
		Vector2f point{};																	// 
		Vector2f relativeVelocity{};														// 

		float bounciness = 0.0f;															// 
		float friction = 0.0f;																// 
		float normalImpulse = 0.0f;															// 
		float separation = 0.0f;															// 
		float tangentImpulse = 0.0f;														// 

		bool enabled = true;																// 
	};
}