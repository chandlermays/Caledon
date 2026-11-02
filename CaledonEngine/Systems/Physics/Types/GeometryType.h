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
		Outlines,				// Generates continuous edge chains along the outer boundary of the composite shape
		Polygons				// Generates solid, decomposed convex polygons filling the interior of the composite shape
	};
}