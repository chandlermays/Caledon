/*------------------------------
| File: SimulationMode2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// A selection of mode that control when Caledon executes the 2D physics simulation

namespace CaledonEngine
{
	enum class SimulationMode2D : uint8_t
	{
		FixedUpdate,							// Execute the physics simulation immediately after FixedUpdate
		Update,									// Execute the physics simulation immediately after Update
		Script									// Execute the physics simulation manually via Physics2D.Simulate
	};
}