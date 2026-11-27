/*------------------------------
| File: Color.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <cstdint>

namespace CaledonEngine
{
	/*------------------------------------------------
	| --- Color: An 8-bit-per-channel RGBA color --- |
	------------------------------------------------*/
	struct Color
	{
		std::uint8_t r = 255;																		// Red channel (0-255)
		std::uint8_t g = 255;																		// Green channel (0-255)
		std::uint8_t b = 255;																		// Blue channel (0-255)
		std::uint8_t a = 255;																		// Alpha channel (0 = transparent, 255 = opaque)

		constexpr Color() = default;																// Constructor (opaque white)
		constexpr Color(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue, const std::uint8_t alpha)	// Parameterized Constructor
			: r{ red }
			, g{ green }
			, b{ blue }
			, a{ alpha }
		{
		}

		constexpr bool operator==(const Color& other) const											// Returns whether every channel matches
		{
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}

		constexpr bool operator!=(const Color& other) const											// Returns whether any channel differs
		{
			return !(*this == other);
		}

		static constexpr Color Red() { return { 255, 0, 0, 255 }; }									// Opaque red
		static constexpr Color Green() { return { 0, 255, 0, 255 }; }								// Opaque green
		static constexpr Color Blue() { return { 0, 0, 255, 255 }; }								// Opaque blue
		static constexpr Color White() { return { 255, 255, 255, 255 }; }							// Opaque white
		static constexpr Color Black() { return { 0, 0, 0, 255 }; }									// Opaque black
		static constexpr Color Yellow() { return { 255, 255, 0, 255 }; }							// Opaque yellow
		static constexpr Color Magenta() { return { 255, 0, 255, 255 }; }							// Opaque magenta
		static constexpr Color Cyan() { return { 0, 255, 255, 255 }; }								// Opaque cyan
		static constexpr Color Orange() { return { 255, 165, 0, 255 }; }							// Opaque orange
		static constexpr Color Purple() { return { 128, 0, 128, 255 }; }							// Opaque purple
		static constexpr Color Gray() { return { 128, 128, 128, 255 }; }							// Opaque gray
	};
}