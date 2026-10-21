/*------------------------------
| File: GenerationType.h
| Author: Chandler Mays
------------------------------*/
#pragma once

// Specifies when to generate the Composite Collider geometry
// Controls when the Composite Collider geometry is updated when a Collider used by a Composite Collider changes

namespace CaledonEngine
{
	enum GenerationType
	{
		Synchronous,				// Update synchronously immediately when a Collider used by the Composite Collider changes
		Manual						// Do not automatically update when a Collider used by the Composite Collider changes

		// ..expand for Caledon's capabilities
	};
}