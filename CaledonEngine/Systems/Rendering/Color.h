/*------------------------------
| File: Color.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <cstdint>

namespace CaledonEngine
{
	struct Color
	{
		std::uint8_t r = 255;
		std::uint8_t g = 255;
		std::uint8_t b = 255;
		std::uint8_t a = 255;

		constexpr Color() = default;
		constexpr Color(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue, const std::uint8_t alpha)
			: r{ red }
			, g{ green }
			, b{ blue }
			, a{ alpha }
		{
		}

		constexpr bool operator==(const Color& other) const
		{
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}

		constexpr bool operator!=(const Color& other) const
		{
			return !(*this == other);
		}

		static constexpr Color Red() { return { 255, 0, 0, 255 }; }
		static constexpr Color Green() { return { 0, 255, 0, 255 }; }
		static constexpr Color Blue() { return { 0, 0, 255, 255 }; }
		static constexpr Color White() { return { 255, 255, 255, 255 }; }
		static constexpr Color Black() { return { 0, 0, 0, 255 }; }
		static constexpr Color Yellow() { return { 255, 255, 0, 255 }; }
		static constexpr Color Magenta() { return { 255, 0, 255, 255 }; }
		static constexpr Color Cyan() { return { 0, 255, 255, 255 }; }
		static constexpr Color Orange() { return { 255, 165, 0, 255 }; }
		static constexpr Color Purple() { return { 128, 0, 128, 255 }; }
		static constexpr Color Gray() { return { 128, 128, 128, 255 }; }
	};
}