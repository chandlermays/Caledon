/*------------------------------
| File: GeometryType.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// Specifies the type of geometry the Composite Collider generates

namespace CaledonEngine
{
	enum class GeometryType : uint8_t
	{
		Outlines,
		Polygons
	};
}