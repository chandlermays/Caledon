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

	/*-----------------------------------------------------------------------------------
	| --- CompositeCollider2D: Combines the shapes of other Collider2Ds (the        --- |
	| ---                      composited colliders) into one set of paths. Any     --- |
	| ---                      change to a setting or to the collider list marks    --- |
	| ---                      the geometry dirty; with Synchronous generation it   --- |
	| ---                      is rebuilt right away, otherwise it waits for a call --- |
	| ---                      to GenerateGeometry. Vertices closer than the vertex --- |
	| ---                      distance are welded together. Each polygon shape     --- |
	| ---                      currently becomes its own path (overlapping shapes   --- |
	| ---                      are not merged yet)                                  --- |
	-----------------------------------------------------------------------------------*/
	class CompositeCollider2D : public Collider2D
	{
	private:
		struct PathRange																																// Where one path sits in the shared vertex list
		{
			int start;																																	// Index of the path's first vertex
			int count;																																	// Number of vertices in the path
		};																																				   
																																						   
		float m_edgeRadius;																																// Radius added around the generated edges
		float m_offsetDistance;																															// Distance used when offsetting the generated geometry
		float m_vertexDistance;																															// Vertices closer than this are welded into one
		GenerationType m_generationType;																												// Whether geometry regenerates on every change or only when asked
		GeometryType m_geometryType;																													// Whether the geometry is generated as outlines or polygons
		bool m_useDelaunayMesh;																															// Whether polygons are triangulated with a Delaunay mesh
		bool m_isGeometryDirty;																															// Flag to indicate the geometry is out of date

		ColliderArray2D m_compositedColliders;																											// Non-owning pointers to the colliders whose shapes are combined
		std::vector<Vector2f> m_pathVertices;																											// Every path's vertices, stored back to back
		std::vector<PathRange> m_paths;																													// Where each path sits in m_pathVertices

		void OnChanged();																																// Marks the geometry dirty and regenerates it when generation is synchronous
		void MergeShapesIntoPaths(const PhysicsShapeGroup2D& shapes);																					// Turns each polygon shape into a welded path
																																						   
	public:																																				   
		CompositeCollider2D();																															// Constructor
		~CompositeCollider2D() override = default;																										// Destructor
		CompositeCollider2D(const CompositeCollider2D&) = delete;																						// Prevent copy-construction
		CompositeCollider2D& operator=(const CompositeCollider2D&) = delete;																			// Prevent copy-assignment
		CompositeCollider2D(CompositeCollider2D&&) = delete;																							// Prevent move-construction
		CompositeCollider2D& operator=(CompositeCollider2D&&) = delete;																					// Prevent move-assignment
																																						   
		const std::string& GetTypeName() const override;																								// Returns the type name of the Component
																								   
		float GetEdgeRadius() const noexcept											{ return m_edgeRadius; }										// Returns the edge radius of this CompositeCollider2D
		GenerationType GetGenerationType() const noexcept								{ return m_generationType; }									// Returns the generation type of this CompositeCollider2D
		GeometryType GetGeometryType() const noexcept									{ return m_geometryType; }										// Returns the geometry type of this CompositeCollider2D
		float GetOffsetDistance() const noexcept										{ return m_offsetDistance; }									// Returns the offset distance of this CompositeCollider2D
		float GetVertexDistance() const noexcept										{ return m_vertexDistance; }									// Returns the vertex distance of this CompositeCollider2D
		bool GetUseDelaunayMesh() const noexcept										{ return m_useDelaunayMesh; }									// Returns whether this CompositeCollider2D uses a Delaunay mesh for triangulation

		void SetEdgeRadius(float edgeRadius);																											// Sets the edge radius (clamped to 0 or more)
		void SetGenerationType(GenerationType generationType);																							// Sets the generation type, regenerating now if it became synchronous
		void SetGeometryType(GeometryType geometryType);																								// Sets the geometry type
		void SetOffsetDistance(float offsetDistance);																									// Sets the offset distance (clamped to 0 or more)
		void SetUseDelaunayMesh(bool useDelaunayMesh);																									// Sets whether a Delaunay mesh is used for triangulation
		void SetVertexDistance(float vertexDistance);																									// Sets the vertex weld distance (clamped to 0 or more)
																								   
		int GetPathCount() const noexcept												{ return static_cast<int>(m_paths.size()); }					// Returns the number of generated paths
		int GetPointCount() const noexcept												{ return static_cast<int>(m_pathVertices.size()); }				// Returns the number of vertices across every path
		bool IsGeometryDirty() const noexcept											{ return m_isGeometryDirty; }									// Returns whether the geometry is out of date
																								   
		void GenerateGeometry();																														// Rebuilds the paths from the composited colliders' shapes
																												   
		ColliderArray2D GetCompositedColliders() const									{ return m_compositedColliders; }								// Returns a copy of the composited colliders
		int GetCompositedColliders(std::vector<Collider2D*>& colliders) const;																			// Copies the composited colliders into a list; returns how many
																															   
		std::span<const Vector2f> GetPath(int index) const noexcept;																					// Returns a path's vertices (empty if the index is invalid)
		void GetPath(int index, std::vector<Vector2f>& points) const;																					// Copies a path's vertices into a list
		int GetPathPointCount(int index) const noexcept;																								// Returns the number of vertices in a path
																															   
		bool AddCompositedCollider(Collider2D* pCollider);																								// Adds a collider to combine; false if null, itself or already added
		bool RemoveCompositedCollider(Collider2D* pCollider);																							// Removes a combined collider; false if it wasn't added
		int AddCompositedColliders(std::span<Collider2D* const> colliders);																				// Adds several colliders to combine; returns how many were added
	};
}