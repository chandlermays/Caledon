/*------------------------------
| File: EdgeCollider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Components/Collider2D.h"

#include <vector>

// Collider for 2D physics representing a free-form chain of line segments (a floor, a wall, a slope, a loop)
//
// * The points are connected in order. To make a closed loop, repeat the first point at the end
// * Edges are infinitely thin (unless edgeRadius is set), have no mass and are two-sided
// * Each segment is its own shape: a chain is not convex, so it can't be one shape
// * Bodies are pushed out of the side they are on. A fast body can still go through a thin edge in a single step (there is no continuous collision yet)

namespace CaledonEngine
{
	class EdgeCollider2D : public Collider2D
	{
	private:
		std::vector<Vector2f> m_points;																							// The points of the chain, in the local space of the collider
		float m_edgeRadius;																										// Controls the radius of all edges created by the collider

	protected:
		int AddLocalShapes(PhysicsShapeGroup2D& physicsShapeGroup) const override;												// Adds the local shapes of this EdgeCollider2D to the physics shape group

	public:
		EdgeCollider2D();																										// Constructor
		~EdgeCollider2D() override = default;																					// Destructor
		EdgeCollider2D(const EdgeCollider2D&) = delete;																			// Prevent copy-construction
		EdgeCollider2D& operator=(const EdgeCollider2D&) = delete;																// Prevent copy-assignment
		EdgeCollider2D(EdgeCollider2D&&) = delete;																				// Prevent move-construction
		EdgeCollider2D& operator=(EdgeCollider2D&&) = delete;																	// Prevent move-assignment

		const std::string& GetTypeName() const override;																		// Returns the type name of this Component

		float GetEdgeRadius() const noexcept							{ return m_edgeRadius; }								// Returns the edge radius of this EdgeCollider2D
		void SetEdgeRadius(float edgeRadius);																					// Sets the edge radius of this EdgeCollider2D

		int GetPointCount() const noexcept								{ return static_cast<int>(m_points.size()); }			// Returns the number of points in the chain for this EdgeCollider2D
		const std::vector<Vector2f>& GetPoints() const noexcept			{ return m_points; }									// Returns the points of the chain for this EdgeCollider2D
		void SetPoints(const std::vector<Vector2f>& points);																	// Sets the points of the chain for this EdgeCollider2D
		void Reset();																											// Resets the points of the chain for this EdgeCollider2D to an empty state
	};
}
