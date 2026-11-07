/*------------------------------
| File: CompositeCollider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/Types/GeometryType.h"
#include "Systems/Physics/Types/GenerationType.h"
#include "Utilities/Math/Vector2.h"

#include <span>
#include <vector>

namespace CaledonEngine
{
	class PhysicsShapeGroup2D;

	using ColliderArray2D = std::vector<Collider2D*>;

	class CompositeCollider2D : public Collider2D
	{
	private:
		struct PathRange																																//
		{
			int start;																																	// 
			int count;																																	// 
		};																																				   
																																						   
		float m_edgeRadius;																																// 
		float m_offsetDistance;																															//
		float m_vertexDistance;																															// 
		GenerationType m_generationType;																												// 
		GeometryType m_geometryType;																													// 
		bool m_useDelaunayMesh;																															//
		bool m_isGeometryDirty;																															// 

		ColliderArray2D m_compositedColliders;																											// 
		std::vector<Vector2f> m_pathVertices;																											// 
		std::vector<PathRange> m_paths;																													// 

		void OnChanged();																																// 
		void MergeShapesIntoPaths(const PhysicsShapeGroup2D& shapes);																					// 
																																						   
	public:																																				   
		CompositeCollider2D();																															// 
		~CompositeCollider2D() override = default;																										// 
		CompositeCollider2D(const CompositeCollider2D&) = delete;																						// 
		CompositeCollider2D& operator=(const CompositeCollider2D&) = delete;																			// 
		CompositeCollider2D(CompositeCollider2D&&) = delete;																							// 
		CompositeCollider2D& operator=(CompositeCollider2D&&) = delete;																					// 
																																						   
		const std::string& GetTypeName() const override;																								// 
																								   
		float GetEdgeRadius() const noexcept											{ return m_edgeRadius; }										// Returns the edge radius of this CompositeCollider2D
		GenerationType GetGenerationType() const noexcept								{ return m_generationType; }									// Returns the generation type of this CompositeCollider2D
		GeometryType GetGeometryType() const noexcept									{ return m_geometryType; }										// Returns the geometry type of this CompositeCollider2D
		float GetOffsetDistance() const noexcept										{ return m_offsetDistance; }									// Returns the offset distance of this CompositeCollider2D
		float GetVertexDistance() const noexcept										{ return m_vertexDistance; }									// Returns the vertex distance of this CompositeCollider2D
		bool GetUseDelaunayMesh() const noexcept										{ return m_useDelaunayMesh; }									// Returns whether this CompositeCollider2D uses a Delaunay mesh for triangulation

		void SetEdgeRadius(float edgeRadius);																											// 
		void SetGenerationType(GenerationType generationType);																							// 
		void SetGeometryType(GeometryType geometryType);																								// 
		void SetOffsetDistance(float offsetDistance);																									// 
		void SetUseDelaunayMesh(bool useDelaunayMesh);																									// 
		void SetVertexDistance(float vertexDistance);																									// 
																								   
		int GetPathCount() const noexcept												{ return static_cast<int>(m_paths.size()); }					// 
		int GetPointCount() const noexcept												{ return static_cast<int>(m_pathVertices.size()); }				// 
		bool IsGeometryDirty() const noexcept											{ return m_isGeometryDirty; }									// 
																								   
		void GenerateGeometry();																														// 
																												   
		ColliderArray2D GetCompositedColliders() const									{ return m_compositedColliders; }								// 
		int GetCompositedColliders(std::vector<Collider2D*>& colliders) const;																			// 
																															   
		std::span<const Vector2f> GetPath(int index) const noexcept;																					// 
		void GetPath(int index, std::vector<Vector2f>& points) const;																					// 
		int GetPathPointCount(int index) const noexcept;																								//
																															   
		bool AddCompositedCollider(Collider2D* pCollider);																								//
		bool RemoveCompositedCollider(Collider2D* pCollider);																							//
		int AddCompositedColliders(std::span<Collider2D* const> colliders);																				//
	};
}