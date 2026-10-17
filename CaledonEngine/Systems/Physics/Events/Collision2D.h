/*------------------------------
| File: Collision2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Scene/Transform.h"
#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/Components/RigidBody2D.h"

// Collision details returned by 2D physics callback functions: i.e. OnCollisionEnter2D(Collision2D)
// It details which Collider2D and RigidBody2D pairs are involved in the collision as well as contact points where the Collider2D meet

namespace CaledonEngine
{
	class Collision2D
	{
//	properties:
//		Collider2D m_collider;										// The incoming Collider2D involved in the collision with the otherCollider
//		int m_contactCount;											// Gets the number of contacts for this collision
//		bool m_enabled;												// Indicates whether the collision response or reaction is enabled or disabled
//		GameObject m_gameObject;									// The incoming GameObject involved in the collision
//		Collider2D m_otherCollider;									// The other Collider2D involved in the collision with the collider
//		RigidBody2D m_otherRigidBody;								// The other RigidBody2D involved in the collision with the rigidbody
//		Vector2 m_relativeVelocity;									// The relative linear velocity of the two colliding obejcts
//		RigidBody2D m_rigidBody;									// The incoming RigidBody2D involved in the collision with the otherRigidbody
//		Transform m_transform;										// The Transform of the incoming object involved in the collision

	public:
		// ContactPoint2D GetContact(int index);					// Gets the contact point at the specified index
		// int GetContacts(ContactPoint2D[] contacts);				// Retrieves all contact points for contacts between collider and otherCollider
	};
}