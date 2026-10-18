/*------------------------------
| File: PhysicsMaterialCombine2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

// Defines how friction and bounciness values from two PhysicsMaterial2D instances are combined.
// Each material specifies a combine mode for friction and bounciness.
// When the modes differ, the higher-priority mode is used:
// Maximum > Minimum > Multiply > Mean > Average.

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