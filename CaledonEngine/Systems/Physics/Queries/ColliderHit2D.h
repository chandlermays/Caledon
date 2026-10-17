/*------------------------------
| File: ColliderHit2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

// Returns information about 2D Colliders detected by a 2D physics query in the scene
// It contains many pieces of information about a detection result including the Collider2D detected
// Always first check to see if it contains a valid result which indicates that a hit (intersection) was detected

namespace CaledonEngine
{
	struct ColliderHit2D
	{
//		Vector2 centroid					// The world space centroid(center) of the physics query shape when it intersects
//		Collider2D collider					// The Collider2D detected by the physics query
//		float distance						// The distance the physics query traversed before it detected a Collider2D
//		float fraction						// The fraction of the distance specified to the physics query before it detected a Collider2D
//		Vector2 normal						// The surface normal of the detected Collider2D
//		Vector2 point						// The world space position where the physics query shape intersected with the detected Collider2D surface
//		RigidBody2D rigidbody				// The Rigidbody2D that the Collider2D detected by the physics query is attached to
//		Transform transform					// The Transform on the GameObject that the Collider2D is attached to
	};
}