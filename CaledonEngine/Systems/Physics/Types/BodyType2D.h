/*------------------------------
| File: BodyType2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

// The physical behavior type of a RigidBody2D.
// Controls the physical behavior of a RigidBody2D: how it should move, react to forces,
// and interact with the rest of the physics simulation.

namespace CaledonEngine
{
	enum class BodyType2D
	{
		// Reacts to gravity and applied forces. Should be moved using forces and never repositioned
		// explicitly. Collides with all other body types. When an attached Collider2D is set to trigger,
		// it will always produce a trigger for any Collider2D attached to all other body types.
		Dynamic,

		// Stops the RigidBody2D from reacting to gravity or applied forces. Can be moved by setting its
		// linearVelocity/angularVelocity or by being repositioned explicitly. Will only collide with a
		// Dynamic body type. When an attached Collider2D is set to trigger, it will always produce a
		// trigger for any Collider2D attached to all other body types.
		Kinematic,

		// Stops the RigidBody2D from reacting to gravity or applied forces. Should never be repositioned
		// explicitly - it is designed to never move. Will only collide with a Dynamic body type. When an
		// attached Collider2D is set to trigger, it will always produce a trigger for any Collider2D
		// attached to a Dynamic or Kinematic body type.
		Static
	};
}