/*------------------------------
| File: CapsuleCollider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/Types/CapsuleDirection2D.h"

// Collider for 2D physics representing a capsule: a rectangle with a half circle on each of two opposite ends
//
// size is the width and height of the whole capsule. The direction says which way the sides extend:
// * Vertical:   radius = size.x / 2, the straight part is (size.y - size.x) long and runs along the Y axis
// * Horizontal: radius = size.y / 2, the straight part is (size.x - size.y) long and runs along the X axis
// 
// If the straight part is not longer than zero the capsule is just a circle

namespace CaledonEngine
{
	class CapsuleCollider2D : public Collider2D
	{
	private:
		CapsuleDirection2D m_direction;															// The direction that the capsule sides can extend
		Vector2f m_size;																		// The width and height of the capsule

	protected:
		int AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const override;				//

	public:
		CapsuleCollider2D();																	// Constructor
		~CapsuleCollider2D() override = default;												// Destructor
		CapsuleCollider2D(const CapsuleCollider2D&) = delete;									// Prevent copy-construction
		CapsuleCollider2D& operator=(const CapsuleCollider2D&) = delete;						// Prevent copy-assignment
		CapsuleCollider2D(CapsuleCollider2D&&) = delete;										// Prevent move-construction
		CapsuleCollider2D& operator=(CapsuleCollider2D&&) = delete;								// Prevent move-assignment

		const std::string& GetTypeName() const override;										// Returns the type name of this Component

		CapsuleDirection2D GetDirection() const noexcept { return m_direction; }				// Returns the direction of this CapsuleCollider2D
		void SetDirection(CapsuleDirection2D direction);										// Sets the direction of this CapsuleCollider2D

		const Vector2f& GetSize() const noexcept { return m_size; }								// Returns the size of this CapsuleCollider2D
		void SetSize(const Vector2f& size);														// Sets the size of this CapsuleCollider2D
	};
}
