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
		None,								// No composite operation is performed; the collider acts independently
		Merge,								// Combines shapes together using a Boolean OR operation (union), dissolving overlapping interior boundaries
		Intersect,							// Retains only the overlapping region common to the combined shapes (Boolean AND)
		Difference,							// Subtracts one shape's area from another or from the composite result (Boolean NOT)
		Flip								// Composes geometry using a Boolean XOR operation (symmetric difference), excluding areas where shapes overlap
	};
}