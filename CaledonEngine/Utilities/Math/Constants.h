#pragma once

#include <limits>

namespace CaledonEngine::Math
{
	inline constexpr float Infinity = std::numeric_limits<float>::infinity();			// Positive infinity for floating-point calculations
	inline constexpr float Pi = 3.14159265358979323846f;								// The mathematical constant pi (π)
	inline constexpr float TwoPi = 2.0f * Pi;											// Two times pi (2π)
	inline constexpr float HalfPi = Pi / 2.0f;											// Half of pi (π/2)
	inline constexpr float Deg2Rad = Pi / 180.0f;										// Conversion factor from degrees to radians
	inline constexpr float Rad2Deg = 180.0f / Pi;										// Conversion factor from radians to degrees

	constexpr float ToRadians(float degrees) { return degrees * Deg2Rad; }				// Converts degrees to radians
	constexpr float ToDegrees(float radians) { return radians * Rad2Deg; }				// Converts radians to degrees
}