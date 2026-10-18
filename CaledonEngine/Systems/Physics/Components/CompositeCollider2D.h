/*------------------------------
| File: CompositeCollider2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Components/Collider2D.h"

// A Collider that can merge oher Colliders together

namespace CaledonEngine
{
	class CompositeCollider2D : public Collider2D
	{
		/*
		
		properties:
			float edgeRadius															// Controls the radius of all edges created by the Collider.
			GenerationType generationType												// Specifies when to generate the Composite Collider geometry.
			GeometryType geometryType													// Specifies the type of geometry the Composite Collider should generate.
			float offsetDistance														// Vertices are offset by this distance when compositing multiple physic shapes. Any vertices between shapes within this distance are combined.
			int pathCount																// The number of paths in the Collider.
			int pointCount																// Gets the total number of points in all the paths within the Collider.
			bool useDelaunayMesh														// When the value is true, the Collider uses an additional Delaunay triangulation step to produce the Collider mesh. When the value is false, this additional step does not occur.
			float vertexDistance														// Controls the minimum distance allowed between generated vertices.
		
		public methods:
			void GenerateGeometry														// Regenerates the Composite Collider geometry.
			ColliderArray2D GetCompositedColliders(Allocator allocator)					// When any Collider2D is using any composite operation other than Collider2D.CompositeOperation.None then it is used by this CompositeCollider2D.
			int GetCompositedColliders(out List<Collider2D> colliders)					// Overload of the above 'GetCompositedColliders'
			NativeArray<Vector2> GetPath(int index, Allocator allocator)				// Gets a path from the Collider by its index.
			int GetPathPointCount(int index)											// Gets the number of points in the specified path from the Collider by its index.
		*/
	};
}