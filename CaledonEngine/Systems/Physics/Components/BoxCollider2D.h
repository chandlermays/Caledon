/*------------------------------
| File: BoxCollider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Components/Collider2D.h"

// Collider for 2D physics representing an axis-aligned rectangle

namespace CaledonEngine
{
	class BoxCollider2D : public Collider2D
	{
	private:
		bool m_autoTiling;																		// Determines whether the BoxCollider2D's shape is automatically updated based on a SpriteComponent's tiling properties
		float m_edgeRadius;																		// Controls the radius of all edges created by the collider
		Vector2f m_size;																		// The width and height of the rectangle

	protected:
		int AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const override;				//

	public:
		BoxCollider2D();																		// Constructor
		~BoxCollider2D() override = default;													// Destructor
		BoxCollider2D(const BoxCollider2D&) = delete;											// Prevent copy-construction
		BoxCollider2D& operator=(const BoxCollider2D&) = delete;								// Prevent copy-assignment
		BoxCollider2D(BoxCollider2D&&) = delete;												// Prevent move-construction
		BoxCollider2D& operator=(BoxCollider2D&&) = delete;										// Prevent move-assignment

		const std::string& GetTypeName() const override;										// Returns the type name of this Component

		float GetEdgeRadius() const noexcept { return m_edgeRadius; }							// Returns the edge radius of this BoxCollider2D
		void SetEdgeRadius(float edgeRadius);													// Sets the edge radius of this BoxCollider2D

		const Vector2f& GetSize() const noexcept { return m_size; }								// Returns the size of this BoxCollider2D
		void SetSize(const Vector2f& size);														// Sets the size of this BoxCollider2D
	};
}