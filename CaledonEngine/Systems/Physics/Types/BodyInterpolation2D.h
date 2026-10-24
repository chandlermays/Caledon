/*------------------------------
| File: BodyInterpolation2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// Interpolation mode for RigidBody2D objects.

namespace CaledonEngine
{
	enum class BodyInterpolation2D : uint8_t
	{
		None,				// Do not apply any smoothing to the object's movement
		Interpolate,		// Smooth movement based on the object's positions in previous frames
		Extrapolate			// Smooth an object's movement based on an estimate of its position in the next frame
	};
}