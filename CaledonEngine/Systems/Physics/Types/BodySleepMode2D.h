/*------------------------------
| File: BodySleepMode2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// Settings for a RigidBody2D's initial sleep state

namespace CaledonEngine
{
	enum class BodySleepMode2D : uint8_t
	{
		NeverSleep,					// The Rigidbody2D is exempt from going to sleep, maintaining continuous simulation tracking regardless of motion or forces
		StartAwake,					// The Rigidbody2D begins in an active, awake state upon initialization
		StartAsleep					// The Rigidbody2D begins in a sleeping state upon initialization to optimize performance until disturbed
	};
}