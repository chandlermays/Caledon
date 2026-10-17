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
		float m_radius;																			// Radius of the circle

	public:
		CircleCollider2D();																		// Constructor
		~CircleCollider2D() override = default;													// Destructor
		CircleCollider2D(const CircleCollider2D&) = delete;										// Prevent copy-construction
		CircleCollider2D& operator=(const CircleCollider2D&) = delete;							// Prevent copy-assignment
		CircleCollider2D(CircleCollider2D&&) = delete;											// Prevent move-construction
		CircleCollider2D& operator=(CircleCollider2D&&) = delete;								// Prevent move-assignment

		const std::string& GetTypeName() const override;										// Returns the type name of this Component
	};
}