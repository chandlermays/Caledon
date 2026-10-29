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
		std::vector<PhysicsShape2D> m_shapes;																	
		std::vector<Vector2f> m_vertices;																		

		int AddShape(PhysicsShapeType2D type, std::span<const Vector2f> vertices, float radius);				
		bool IsValidShape(int shapeIndex) const noexcept;														

	public:
		explicit PhysicsShapeGroup2D(int shapeCapacity = 1, int vertexCapacity = 8);							

		int GetShapeCount() const noexcept { return static_cast<int>(m_shapes.size()); }						
		int GetVertexCount() const noexcept { return static_cast<int>(m_vertices.size()); }						

		void Add(const PhysicsShapeGroup2D& physicsShapeGroup);													
		int AddBox(const Vector2f& center, const Vector2f& size, float angle = 0.0f, float edgeRadius = 0.0f);	
		int AddCapsule(const Vector2f& vertex0, const Vector2f& vertex1, float radius);							
		int AddCircle(const Vector2f& center, float radius);													
		int AddEdges(std::span<const Vector2f> vertices, float edgeRadius = 0.0f);								
		int AddPolygon(std::span<const Vector2f> vertices);														
		void Clear() noexcept;																					
		bool DeleteShape(int shapeIndex);																		

		const PhysicsShape2D* GetShape(int shapeIndex) const noexcept;											
		std::span<const PhysicsShape2D> GetShapes() const noexcept { return m_shapes; }							
		std::span<const Vector2f> GetVertices() const noexcept { return m_vertices; }							
		void GetShapeData(std::vector<PhysicsShape2D>& shapes, std::vector<Vector2f>& vertices) const;			
		const Vector2f* GetShapeVertex(int shapeIndex, int vertexIndex) const noexcept;
		std::span<const Vector2f> GetShapeVertices(int shapeIndex) const noexcept;								
		void GetShapeVertices(int shapeIndex, std::vector<Vector2f>& vertices) const;							

		bool SetShapeAdjacentVertices(int shapeIndex, bool useAdjStart, bool useAdjEnd,
			const Vector2f& adjStart, const Vector2f& adjEnd);													
		bool SetShapeRadius(int shapeIndex, float radius);														
		bool SetShapeVertex(int shapeIndex, int vertexIndex, const Vector2f& vertex);
	};
}