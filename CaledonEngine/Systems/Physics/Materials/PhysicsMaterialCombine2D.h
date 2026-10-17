/*------------------------------
| File: PhysicsMaterialCombine2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

// Describes how PhysicsMaterial2D friction and bounciness are combined when two Collider2D come into contact

// When two Collider2D come into contact, each might has its own PhysicsMaterial2D assigned, and each with its own .friction and .bounciness
// To calculate the collision response, both friction and bounciness values must be combined using the PhysicsMaterialCombine2D provides multiple algorithms

// NOTE: Each Collider2D can have a unique PhysicsMaterial2D with different combine modes for friction and bounciness.
// When different modes are set, the mode with the highest priority is used in this order: Maximum, Minimum, Multiply, Mean, and Average
// For example, if one PhysicsMaterial2D uses Average while the other uses Maximum, the Maximum combine function is used because it has higher priority

namespace CaledonEngine
{
	enum class PhysicsMaterialCombine2D
	{
		Average,				// Uses an Average algorithm when combining friction or bounciness
		Mean,					// Uses a Geometric Mean algorithm when combining friction or bounciness
		Multiply,				// Uses a Multiply algorithm when combining friction or bounciness i.e. the product of the two values is used
		Minimum,				// Uses a Minimum algorithm when combining friction or bounciness i.e. the minimum of the two values is used
		Maximum					// Uses a Maximum algorithm when combining friction or bounciness i.e. the maximum of the two values is used
	};
}