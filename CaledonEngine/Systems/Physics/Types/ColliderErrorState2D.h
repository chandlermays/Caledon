/*------------------------------
| File: ColliderErrorState2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

// Indicates what (if any) error was encountered when creating a 2D Collider
// This error state can be read directly from the 2D Collider via: Collider2D.errorState

namespace CaledonEngine
{
	enum ColliderErrorState2D
	{
		None,
		NoShapes,
		RemovedShapes,

		// ..expand for Caledon's capabilities
	};
}