/*------------------------------
| File: ColliderErrorState2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// Indicates what (if any) error was encountered when creating a 2D Collider
// This error state can be read directly from the 2D Collider via: Collider2D.errorState

namespace CaledonEngine
{
	enum class ColliderErrorState2D : uint8_t
	{
		None,								// No errors encountered; collider geometry and configuration are fully valid
		NoShapes,							// The collider contains no shapes or failed to generate any valid shape data
		RemovedShapes,						// One or more shapes were automatically filtered out or removed due to being invalid or degenerate
	//  InsufficientVertices?
	
		// ..expand for Caledon's capabilities
	};
}