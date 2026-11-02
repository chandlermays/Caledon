/*------------------------------
| File: CapsuleDirection2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// The direction that the capsule sides can extend

namespace CaledonEngine
{
	enum class CapsuleDirection2D : uint8_t
	{
		Horizontal,						// The capsule sides extend horizontally
		Vertical						// The capsule sides extend vertically
	};
}