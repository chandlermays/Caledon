/*------------------------------
| File: CapsuleCollider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Components/Collider2D.h"

namespace CaledonEngine
{
	class CapsuleCollider2D : public Collider2D
	{
		// CapsuleDirection2D m_direction { get; set; }
		// Vector2f m_size { get; set; }

	protected:
		int AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const override;					//

	public:
		CapsuleCollider2D();																		// Constructor
		~CapsuleCollider2D() override = default;													// Destructor
		CapsuleCollider2D(const CapsuleCollider2D&) = delete;										// Prevent copy-construction
		CapsuleCollider2D& operator=(const CapsuleCollider2D&) = delete;							// Prevent copy-assignment
		CapsuleCollider2D(CapsuleCollider2D&&) = delete;											// Prevent move-construction
		CapsuleCollider2D& operator=(CapsuleCollider2D&&) = delete;									// Prevent move-assignment

		const std::string& GetTypeName() const override;											// Returns the type name of this Component

		// GetDirection
		// SetDirection
		
		// GetSize
		// SetSize
	};
}