/*------------------------------
| File: CollisionDetectionMode2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

// Controls how collisions are detected when a RigidBody2D moves

namespace CaledonEngine
{
	enum class CollisionDetectionMode2D
	{
		Discrete,			// When a RigidBody2D moves, only collisions at the new position are detected
		Continuous			// Ensures that all collisions are detected when a RigidBody2D moves
	};
}