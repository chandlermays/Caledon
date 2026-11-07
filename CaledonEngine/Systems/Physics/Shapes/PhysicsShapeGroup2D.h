/*------------------------------
| File: PhysicsShapeGroup2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/PhysicsShape2D.h"
#include "Utilities/Math/Vector2.h"

#include <vector>
#include <span>

// Represents a group of PhysicsShape2D and their geometry
// 
// A shape group represents multiple PhysicsShape2D of the same or mixed PhysicsShapeType2D along with their geometry. It is comprised of a single list of vertices (GetShapeVertices)
// along with a list of PhysicsShape2D which refer to specific ranges of those vertices i.e. they index into the list of vertices
// Some shape types use a fixed number of vertices and some use a variable number of vertices therefore this sinle vertices list is a compact
// and efficient representation for multiple PhysicsShape2D in a group
//
// The shape group can be created by using the following methods:
// * Calling Collider2D.GetShapes where it would then represent all the shapes produced by that Collider2D
// * Calling RigidBody2D.GetShapes where it would then represent all the shapes produced by all the Collider2D attached to that RigidBody2D
// * Manually populating with custom shapes by calling AddCircle, AddCapsule, AddPolygon, AddBox, or AddEdges

namespace CaledonEngine
{
	class PhysicsShapeGroup2D
	{
	private:
		// Matrix4v4 m_localToWorldMatrix;
		std::vector<PhysicsShape2D> m_shapes;																														// List of PhysicsShape2D in this PhysicsShapeGroup2D
		std::vector<Vector2f> m_vertices;																															// List of vertices used by the PhysicsShape2D in this PhysicsShapeGroup2D

		int AddShape(PhysicsShapeType2D type, std::span<const Vector2f> vertices, float radius);																	// Adds a shape to this PhysicsShapeGroup2D
		bool IsValidShape(int shapeIndex) const noexcept;																											// Returns true if the shape index is valid for this PhysicsShapeGroup2D

	public:
		explicit PhysicsShapeGroup2D(int shapeCapacity = 1, int vertexCapacity = 8);																				// Parameterized Constructor

		int GetShapeCount() const noexcept																	{ return static_cast<int>(m_shapes.size()); }			// Returns the number of PhysicsShape2D in this PhysicsShapeGroup2D
		int GetVertexCount() const noexcept																	{ return static_cast<int>(m_vertices.size()); }			// Returns the number of vertices in this PhysicsShapeGroup2D

		void Add(const PhysicsShapeGroup2D& physicsShapeGroup);																										// Adds the shapes and vertices from another PhysicsShapeGroup2D to this PhysicsShapeGroup2D
		int AddBox(const Vector2f& center, const Vector2f& size, float angle = 0.0f, float edgeRadius = 0.0f);														// Adds a box shape to this PhysicsShapeGroup2D
		int AddCapsule(const Vector2f& vertex0, const Vector2f& vertex1, float radius);																				// Adds a capsule shape to this PhysicsShapeGroup2D
		int AddCircle(const Vector2f& center, float radius);																										// Adds a circle shape to this PhysicsShapeGroup2D
		int AddEdges(std::span<const Vector2f> vertices, float edgeRadius = 0.0f);																					// Adds an edges shape to this PhysicsShapeGroup2D
		int AddPolygon(std::span<const Vector2f> vertices);																											// Adds a polygon shape to this PhysicsShapeGroup2D
		void Clear() noexcept;																																		// Clears all shapes and vertices from this PhysicsShapeGroup2D
		bool DeleteShape(int shapeIndex);																															// Deletes a shape from this PhysicsShapeGroup2D

		const PhysicsShape2D* GetShape(int shapeIndex) const noexcept;																								// Returns a pointer to the PhysicsShape2D at the given index
		std::span<const PhysicsShape2D> GetShapes() const noexcept											{ return m_shapes; }									// Returns a span of all PhysicsShape2D in this PhysicsShapeGroup2D
		std::span<const Vector2f> GetVertices() const noexcept												{ return m_vertices; }									// Returns a span of all vertices in this PhysicsShapeGroup2D
		void GetShapeData(std::vector<PhysicsShape2D>& shapes, std::vector<Vector2f>& vertices) const;																// Copies the shapes and vertices from this PhysicsShapeGroup2D into the provided vectors
		const Vector2f* GetShapeVertex(int shapeIndex, int vertexIndex) const noexcept;																				// Returns a pointer to the vertex at the given index for the shape at the given index
		std::span<const Vector2f> GetShapeVertices(int shapeIndex) const noexcept;																					// Returns a span of the vertices for the shape at the given index
		void GetShapeVertices(int shapeIndex, std::vector<Vector2f>& vertices) const;																				// Copies the vertices for the shape at the given index into the provided vector

		bool SetShapeAdjacentVertices(int shapeIndex, bool useAdjStart, bool useAdjEnd,																				// Sets the adjacent vertices for the shape at the given index
			const Vector2f& adjStart, const Vector2f& adjEnd);

		bool SetShapeRadius(int shapeIndex, float radius);																											// Sets the radius for the shape at the given index
		bool SetShapeVertex(int shapeIndex, int vertexIndex, const Vector2f& vertex);																				// Sets the vertex at the given index for the shape at the given index
	};
}