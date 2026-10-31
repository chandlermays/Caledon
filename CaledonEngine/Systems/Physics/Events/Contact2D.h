/*------------------------------
| File: Contact2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/ShapeManifold2D.h"
#include "Utilities/Math/Vector2.h"

#include <cstddef>
#include <functional>

// One contact between a shape of one Collider2D and a shape of another Collider2D, tracked by the PhysicsScene2D from step to step
//
// * A contact is identified by its key: the two colliders plus the index of the shape on each. Shape A always belongs to the collider
//   that was registered first, so the key (and the order of the contact list) is the same every time
// * A "solid" contact has a manifold (normal, contact points, depth). A "trigger" contact only says that the shapes overlap
// * The manifold normal points from collider A to collider B (see ShapeManifold2D)
// * Contacts live inside the PhysicsScene2D. The collider pointers are non-owning

namespace CaledonEngine
{
	class Collider2D;

	struct ContactKey2D
	{
		const Collider2D* pColliderA = nullptr;
		const Collider2D* pColliderB = nullptr;
		int shapeIndexA = 0;
		int shapeIndexB = 0;

		bool operator==(const ContactKey2D& other) const noexcept
		{
			return pColliderA == other.pColliderA && pColliderB == other.pColliderB && shapeIndexA == other.shapeIndexA && shapeIndexB == other.shapeIndexB;
		}
	};

	struct ContactKey2DHash
	{
		std::size_t operator()(const ContactKey2D& key) const noexcept
		{
			std::size_t hash = std::hash<const void*>{}(key.pColliderA);
			hash = hash * 31u + std::hash<const void*>{}(key.pColliderB);
			hash = hash * 31u + static_cast<std::size_t>(key.shapeIndexA);
			hash = hash * 31u + static_cast<std::size_t>(key.shapeIndexB);
			return hash;
		}
	};

	struct Contact2D
	{
		Collider2D* pColliderA = nullptr;																			// The first collider (registered before B)
		Collider2D* pColliderB = nullptr;																			// The second collider
		int shapeIndexA = 0;																						// Which shape of A is touching
		int shapeIndexB = 0;																						// Which shape of B is touching

		bool isTrigger = false;																						// Whether this is a trigger overlap (no manifold) instead of a solid contact
		bool isNew = false;																							// Whether this contact began in the latest update (it did not exist in the one before)

		ShapeManifold2D manifold{ };																				// The contact geometry (solid contacts only); the normal points from A to B
		Vector2f velocityA[2]{ };																					// The velocity of collider A's body at each manifold point (before the step)
		Vector2f velocityB[2]{ };																					// The velocity of collider B's body at each manifold point (before the step)

		float friction = 0.0f;																						// The friction of the two materials, combined
		float bounciness = 0.0f;																					// The bounciness of the two materials, combined

		ContactKey2D GetKey() const noexcept { return ContactKey2D{ pColliderA, pColliderB, shapeIndexA, shapeIndexB }; }
	};
}
