#pragma once

namespace CaledonEngine::Math
{
	inline constexpr float PI = 3.14159265358979323846f;							// The mathematical constant pi (π)
	inline constexpr float TWO_PI = 2.0f * PI;										// Two times pi (2π)
	inline constexpr float HALF_PI = PI / 2.0f;										// Half of pi (π/2)
	inline constexpr float DEG2RAD = PI / 180.0f;									// Conversion factor from degrees to radians
	inline constexpr float RAD2DEG = 180.0f / PI;									// Conversion factor from radians to degrees

	constexpr float ToRadians(float degrees) { return degrees * DEG2RAD; }			// Converts degrees to radians
	constexpr float ToDegrees(float radians) { return radians * RAD2DEG; }			// Converts radians to degrees
}