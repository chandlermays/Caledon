/*------------------------------
| File: ForceMode2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// Option for how to apply a force using RigidBody2D::AddForce

namespace CaledonEngine
{
	enum class ForceMode2D : uint8_t
	{
		Force,			// Add a force to the RigidBody2D, using its mass
		Impulse			// Add an instant force impulse to the RigidBody2D, using its mass
	};
}