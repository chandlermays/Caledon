/*------------------------------
| File: CircleCollider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Physics/Components/Collider2D.h"

namespace CaledonEngine
{
	class CircleCollider2D : public Collider2D
	{
	private:
		float m_radius;																				// The radius of this CircleCollider2D

	protected:
		int AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const override;					//

	public:
		CircleCollider2D();																			// Constructor
		~CircleCollider2D() override = default;														// Destructor
		CircleCollider2D(const CircleCollider2D&) = delete;											// Prevent copy-construction
		CircleCollider2D& operator=(const CircleCollider2D&) = delete;								// Prevent copy-assignment
		CircleCollider2D(CircleCollider2D&&) = delete;												// Prevent move-construction
		CircleCollider2D& operator=(CircleCollider2D&&) = delete;									// Prevent move-assignment

		const std::string& GetTypeName() const override;											// Returns the type name of this Component

		float GetRadius() const noexcept { return m_radius; }										// Returns the radius of this CircleCollider2D
		void SetRadius(float radius);																// Sets the radius of this CircleCollider2D
	};
}