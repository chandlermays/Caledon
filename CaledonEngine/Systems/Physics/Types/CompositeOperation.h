/*------------------------------
| File: CompositeOperation.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// Specifies the composite operation to be used by a Collider2D

namespace CaledonEngine
{
	enum class CompositeOperation : uint8_t
	{
		None,
		Merge,
		Intersect,
		Difference,
		Flip,

		//..expand for Caledon's capabilities
	};
}