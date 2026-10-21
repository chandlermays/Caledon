/*------------------------------
| File: PhysicsShape2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/PhysicsShapeType2D.h"
#include "Utilities/Math/Vector2.h"

// Represents an efficient low-level physics shape used by the physics subsystem
// A Collider2D is a high-level representation of physics geomtry that produces low-level PhysicsShape2D geometry that the physics subsystem understands
// A PhysicsShape2D can represent any shape type as defined by PhysicsShapeType2D

namespace CaledonEngine
{
	struct PhysicsShape2D
	{
		Vector2f adjacentStart{};				// Defines the position of a virtual point adjacent to the start vertex of an edge shape
		Vector2f adjacentEnd{};					// Defines the position of a virtual point adjacent to the end vertex of an edge shape
		float radius = 0.0f;					// The radius of the shape
		int vertexCount = 0;					// The total number of vertices used to represent the shape type
		int vertexStartIndex = 0;				// The start index for the geometry of this shape within the PhysicsShapeGroup2D
		PhysicsShapeType2D shapeType;			// The shape type determines how the vertices and radius are used by this PhysicsShape2D
		bool useAdjacentStart;					// Whether the shape uses the adjacentStart feature
		bool useAdjacentEnd;					// Whether the shape uses the adjacentEnd feaure
	};
}