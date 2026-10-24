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
		NeverSleep,			//
		StartAwake,			// 
		StartAsleep			// 
	};
}