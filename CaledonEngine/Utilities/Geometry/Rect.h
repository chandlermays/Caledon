/*------------------------------
| File: Rect.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Utilities/Math/Vector2.h"

#include <string>

namespace CaledonEngine
{
	template <typename T>
	struct Rect
	{
		static_assert(std::is_arithmetic<T>::value, "Rect can only be instantiated with arithmetic types (int, float, double, etc.)");

		T m_x;																							// The x position (left) of the rectangle
		T m_y;																							// The y position (top) of the rectangle
		T m_width;																						// The width of the rectangle
		T m_height;																						// The height of the rectangle

		constexpr Rect(T xPos = T{}, T yPos = T{}, T w = T{}, T h = T{})								// Constructs a rectangle with explicit position and size
			: m_x{ xPos }
			, m_y{ yPos }
			, m_width{ w }
			, m_height{ h }
		{
		}

		constexpr Rect(const Vector2<T>& position, const Vector2<T>& size)								// Constructs a rectangle from a position vector and a size vector
			: m_x{ position.x }
			, m_y{ position.y }
			, m_width{ size.x }
			, m_height{ size.y }
		{
		}

		constexpr bool operator==(const Rect& other) const												// Compare two rectangles for equality
		{
			return m_x == other.m_x && m_y == other.m_y &&
				m_width == other.m_width && m_height == other.m_height;
		}

		constexpr bool operator!=(const Rect& other) const												// Compare two rectangles for inequality
		{
			return !(*this == other);
		}

		constexpr T Left() const { return m_x; }									// Returns the left (x) coordinate of the rectangle
		constexpr T Right() const { return m_x + m_width; }						// Returns the right (x + width) coordinate of the rectangle
		constexpr T Top() const { return m_y; }									// Returns the top (y) coordinate of the rectangle
		constexpr T Bottom() const { return m_y + m_height; }						// Returns the bottom (y + height) coordinate of the rectangle

		constexpr T CenterX() const { return m_x + m_width / 2; }					// Returns the X coordinate of the rectangle center
		constexpr T CenterY() const { return m_y + m_height / 2; }					// Returns the Y coordinate of the rectangle center

		constexpr Vector2<T> GetPosition() const { return { m_x, m_y }; }						// Returns the position as a Vector2 (x, y)
		constexpr Vector2<T> GetSize() const { return { m_width, m_height }; }				// Returns the size as a Vector2 (width, height)
		constexpr Vector2<T> GetCenter() const { return { CenterX(), CenterY() }; }			// Returns the center as a Vector2 (centerX, centerY)

		/*---------------------------------------------------------------------------------------
		| --- Contains: Returns true if the point (pointX, pointY) is inside this rectangle --- |
		---------------------------------------------------------------------------------------*/
		constexpr bool Contains(T pointX, T pointY) const
		{
			return pointX >= m_x && pointX < m_x + m_width &&
				pointY >= m_y && pointY < m_y + m_height;
		}

		/*-----------------------------------------------------------------------------------------
		| --- Contains: Returns true if the point (point.x, point.y) is inside this rectangle --- |
		-----------------------------------------------------------------------------------------*/
		constexpr bool Contains(const Vector2<T>& point) const
		{
			return Contains(point.x, point.y);
		}

		/*---------------------------------------------------------------------------------------------
		| --- Contains: Returns true if the `other` rectangle is completely inside this rectangle --- |
		---------------------------------------------------------------------------------------------*/
		constexpr bool Contains(const Rect& other) const
		{
			return other.m_x >= m_x && other.Right() <= Right() &&
				other.m_y >= m_y && other.Bottom() <= Bottom();
		}

		/*------------------------------------------------------------------------------------------
		| --- Intersects: Returns true if this rectangle intersects with the `other` rectangle --- |
		------------------------------------------------------------------------------------------*/
		constexpr bool Intersects(const Rect& other) const
		{
			return !(Right() <= other.Left() || Left() >= other.Right() ||
				Bottom() <= other.Top() || Top() >= other.Bottom());
		}

		/*----------------------------------------------------------------------------
		| --- Scale: Scales the rectangle's width and height by the given factor --- |
		----------------------------------------------------------------------------*/
		void Scale(float factor)
		{
			m_width = static_cast<T>(m_width * factor);
			m_height = static_cast<T>(m_height * factor);
		}

		/*----------------------------------------------------------------------------------
		| --- ToString: Returns a string representation of the rectangle in the format --- |
		----------------------------------------------------------------------------------*/
		std::string ToString() const
		{
			return "{x:" + std::to_string(m_x) + ", y:" + std::to_string(m_y) +
				", w:" + std::to_string(m_width) + ", h:" + std::to_string(m_height) + "}";
		}
	};

	// Type aliases for common rect types
	using RectInt = Rect<int>;
	using RectFloat = Rect<float>;
}