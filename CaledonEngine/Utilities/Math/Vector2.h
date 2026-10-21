/*------------------------------
| File: Vector2.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Constants.h"

#include <cmath>
#include <algorithm>
#include <cassert>
#include <limits>
#include <string>
#include <type_traits>

namespace CaledonEngine
{
	template <typename T>
	class Vector2
	{
		static_assert(std::is_arithmetic<T>::value, "Vector2 can only be instantiated with arithmetic types (int, float, double, etc.)");

	public:
		static constexpr size_t N = 2;			// Number of components in the vector

		T x;									// The x component of the vector
		T y;									// The y component of the vector

		/*----------------------
		| --- Constructors --- |
		----------------------*/
		constexpr inline Vector2()										// Default constructor initializes the vector to (0, 0)
			: x{ 0 }
			, y{ 0 }
		{
		}

		constexpr inline Vector2(const Vector2& other) = default;		// Copy constructor

		constexpr inline Vector2(const T x, const T y)					// Constructor that initializes the vector with specified x and y values
			: x{ x }
			, y{ y }
		{
		}

		constexpr inline explicit Vector2(const T uniformValue)			// Constructor that initializes both components of the vector to the same value
			: x{ uniformValue }
			, y{ uniformValue }
		{
		}

		/*--------------------
		| --- Operators  --- |
		--------------------*/
		constexpr inline Vector2& operator=(const Vector2& other) = default;														// Copy assignment operator

		constexpr inline bool operator==(const Vector2 other) const { return (x == other.x && y == other.y); }				// Compare two vectors for equality
		constexpr inline bool operator!=(const Vector2 other) const { return !(*this == other); }							// Compare two vectors for inequality
		constexpr inline bool operator<(const Vector2 other) const { return (x < other.x && y < other.y); }				// Compare if this vector is less than another vector (component-wise)
		constexpr inline bool operator>(const Vector2 other) const { return (x > other.x && y > other.y); }				// Compare if this vector is greater than another vector (component-wise)
		constexpr inline bool operator<=(const Vector2 other) const { return (x <= other.x && y <= other.y); }				// Compare if this vector is less than or equal to another vector (component-wise)
		constexpr inline bool operator>=(const Vector2 other) const { return (x >= other.x && y >= other.y); }				// Compare if this vector is greater than or equal to another vector (component-wise)

		constexpr inline T operator[](const size_t index) const { assert(index < 2); return (index == 0) ? x : y; }		// Access vector components by index (0 for x, 1 for y)
		constexpr inline T& operator[](const size_t index) { assert(index < 2); return (index == 0) ? x : y; }		// Access vector components by index (0 for x, 1 for y)

		constexpr inline Vector2 operator-() const { return { -x, -y }; }									// Negate the vector (unary minus)
		constexpr inline Vector2 operator+(const Vector2 other) const { return { x + other.x, y + other.y }; }				// Add two vectors component-wise
		constexpr inline Vector2 operator-(const Vector2 other) const { return { x - other.x, y - other.y }; }				// Subtract two vectors component-wise
		constexpr inline Vector2 operator*(const Vector2 other) const { return { x * other.x, y * other.y }; }				// Multiply two vectors component-wise
		constexpr inline Vector2 operator/(const Vector2 other) const { return { x / other.x, y / other.y }; }				// Divide two vectors component-wise

		constexpr inline Vector2 operator+(const T scalar) const { return { x + scalar, y + scalar }; }					// Add a scalar to each component of this vector
		constexpr inline Vector2 operator-(const T scalar) const { return { x - scalar, y - scalar }; }					// Subtract a scalar from each component of this vector
		constexpr inline Vector2 operator*(const T scalar) const { return { x * scalar, y * scalar }; }					// Multiply each component of this vector by a scalar
		constexpr inline Vector2 operator/(const T scalar) const { return { x / scalar, y / scalar }; }					// Divide each component of this vector by a scalar

		constexpr inline Vector2& operator+=(const Vector2 other) { return *this = *this + other; }						// Add another vector to this vector component-wise and assign the result to this vector
		constexpr inline Vector2& operator-=(const Vector2 other) { return *this = *this - other; }						// Subtract another vector from this vector component-wise and assign the result to this vector
		constexpr inline Vector2& operator*=(const Vector2 other) { return *this = *this * other; }						// Multiply this vector by another vector component-wise and assign the result to this vector
		constexpr inline Vector2& operator/=(const Vector2 other) { return *this = *this / other; }						// Divide this vector by another vector component-wise and assign the result to this vector

		constexpr inline Vector2& operator+=(const T scalar) { return *this = *this + scalar; }						// Add a scalar to each component of this vector and assign the result to this vector
		constexpr inline Vector2& operator-=(const T scalar) { return *this = *this - scalar; }						// Subtract a scalar from each component of this vector and assign the result to this vector
		constexpr inline Vector2& operator*=(const T scalar) { return *this = *this * scalar; }						// Multiply each component of this vector by a scalar and assign the result to this vector
		constexpr inline Vector2& operator/=(const T scalar) { return *this = *this / scalar; }						// Divide each component of this vector by a scalar and assign the result to this vector

		static constexpr inline Vector2 Up() { return { 0, 1 }; }									// Returns a vector pointing upwards (0, 1)
		static constexpr inline Vector2 Down() { return { 0, -1 }; }									// Returns a vector pointing downwards (0, -1)
		static constexpr inline Vector2 Left() { return { -1, 0 }; }									// Returns a vector pointing leftwards (-1, 0)
		static constexpr inline Vector2 Right() { return { 1, 0 }; }									// Returns a vector pointing rightwards (1, 0)
		static constexpr inline Vector2 One() { return { 1, 1 }; }									// Returns a vector with all components set to 1 (1, 1)
		static constexpr inline Vector2 Zero() { return { 0, 0 }; }									// Returns a vector with all components set to 0 (0, 0)

		constexpr inline T X() const { return x; }											// Get the x component of the vector
		constexpr inline T Y() const { return y; }											// Get the y component of the vector

		/*-----------------------------------------------------
		| --- Magnitude: Returns the length of the vector --- |
		-----------------------------------------------------*/
		inline T Magnitude() const
		{
			return std::sqrt(SqrMagnitude());
		}

		/*----------------------------------------------------------------
		| --- SqrMagnitude: Returns the squared length of the vector --- |
		----------------------------------------------------------------*/
		constexpr inline T SqrMagnitude() const
		{
			return x * x + y * y;
		}

		/*-----------------------------------------------------------------------------------
		| --- Normalized: Returns a vector with the same direction but a magnitude of 1 --- |
		-----------------------------------------------------------------------------------*/
		inline Vector2 Normalized() const
		{
			return *this / Magnitude();
		}

		/*------------------------------------------------------------
		| --- Normalize: Makes this vector have a magnitude of 1 --- |
		------------------------------------------------------------*/
		inline void Normalize()
		{
			*this = Normalized();
		}

		/*------------------------------------------------------------------------
		| --- Equals: Returns whether this vector is equal to another vector --- |
		------------------------------------------------------------------------*/
		constexpr inline bool Equals(const Vector2& other) const
		{
			return *this == other;
		}

		/*--------------------------------------------------------------------------------
		| --- Set: Sets the x and y components of the vector to the specified values --- |
		--------------------------------------------------------------------------------*/
		constexpr inline void Set(const T newX, const T newY)
		{
			x = newX;
			y = newY;
		}

		/*----------------------------------------------------------------------------------------
		| --- ToString: Returns a string representation of the vector in the format "{x, y}" --- |
		----------------------------------------------------------------------------------------*/
		inline std::string ToString() const
		{
			return "{" + std::to_string(x) + ", " + std::to_string(y) + "}";
		}

		/*-------------------------------------------------------------------------
		| --- Perpendicular: Returns a 2D vector perpendicular to this vector --- |
		-------------------------------------------------------------------------*/
		constexpr inline Vector2 Perpendicular() const
		{
			return { -y, x };
		}

		/*----------------------------------------------------------------------
		| --- Floor: Returns a copy of this vector, each component floored --- |
		----------------------------------------------------------------------*/
		inline Vector2 Floor() const
		{
			return { static_cast<T>(std::floor(x)), static_cast<T>(std::floor(y)) };
		}

		/*----------------------------------------------------------------------
		| --- Round: Returns a copy of this vector, each component rounded --- |
		----------------------------------------------------------------------*/
		inline Vector2 Round() const
		{
			return { static_cast<T>(std::round(x)), static_cast<T>(std::round(y)) };
		}

		/*-----------------------------------------------------------------------------
		| --- Rotate: Returns a copy of this vector rotated by an angle (degrees) --- |
		-----------------------------------------------------------------------------*/
		inline Vector2 Rotate(const T angleDegrees) const
		{
			T radians = angleDegrees * static_cast<T>(CaledonEngine::Math::Deg2Rad);
			T cosA = static_cast<T>(std::cos(radians));
			T sinA = static_cast<T>(std::sin(radians));

			return { x * cosA - y * sinA, x * sinA + y * cosA };
		}

		/*--------------------------------------------------------------------------
		| --- Angle: Returns the unsigned angle in degrees between from and to --- |
		--------------------------------------------------------------------------*/
		static inline T Angle(const Vector2& from, const Vector2& to)
		{
			return std::acos(Dot(from, to) / (from.Magnitude() * to.Magnitude())) * static_cast<T>(CaledonEngine::Math::Rad2Deg);
		}

		/*------------------------------------------------------------------------------
		| --- SignedAngle: Returns the signed angle in degrees between from and to --- |
		------------------------------------------------------------------------------*/
		static inline T SignedAngle(const Vector2& from, const Vector2& to)
		{
			T unsignedAngle = Angle(from, to);
			T sign = (Cross(from, to) < 0) ? static_cast<T>(-1) : static_cast<T>(1);
			return unsignedAngle * sign;
		}

		/*------------------------------------------------------------------------------------------
		| --- ClampMagnitude: Returns a copy of vector with its magnitude clamped to maxLength --- |
		------------------------------------------------------------------------------------------*/
		static inline Vector2 ClampMagnitude(const Vector2& vector, const float maxLength)
		{
			if (vector.Magnitude() > maxLength)
			{
				return vector.Normalized() * maxLength;
			}
			return vector;
		}

		/*--------------------------------------------------------
		| --- Distance: Returns the distance between a and b --- |
		--------------------------------------------------------*/
		static inline T Distance(const Vector2& a, const Vector2& b)
		{
			return (a - b).Magnitude();
		}

		/*-------------------------------------------------------------------
		| --- SqrDistance: Returns the squared distance between a and b --- |
		-------------------------------------------------------------------*/
		static constexpr inline T SqrDistance(const Vector2& a, const Vector2& b)
		{
			return (a - b).SqrMagnitude();
		}

		/*-----------------------------------------------------
		| --- Dot: Returns the dot product of two vectors --- |
		-----------------------------------------------------*/
		static constexpr inline T Dot(const Vector2& lhs, const Vector2& rhs)
		{
			return lhs.x * rhs.x + lhs.y * rhs.y;
		}

		/*---------------------------------------------------------------------
		| --- Cross: Returns the 2D (scalar) cross product of two vectors --- |
		---------------------------------------------------------------------*/
		static constexpr inline T Cross(const Vector2& lhs, const Vector2& rhs)
		{
			return lhs.x * rhs.y - lhs.y * rhs.x;
		}

		/*-------------------------------------------------------------------------------
		| --- Max: Returns a vector made from the largest components of two vectors --- |
		-------------------------------------------------------------------------------*/
		static constexpr inline Vector2 Max(const Vector2& lhs, const Vector2& rhs)
		{
			return { std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y) };
		}

		/*--------------------------------------------------------------------------------
		| --- Min: Returns a vector made from the smallest components of two vectors --- |
		--------------------------------------------------------------------------------*/
		static constexpr inline Vector2 Min(const Vector2& lhs, const Vector2& rhs)
		{
			return { std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y) };
		}

		/*------------------------------------------------------------------------
		| --- Clamp: Clamps each component of the vector between min and max --- |
		------------------------------------------------------------------------*/
		static constexpr inline Vector2 Clamp(const Vector2& vector, const Vector2& min, const Vector2& max)
		{
			return { std::clamp(vector.x, min.x, max.x), std::clamp(vector.y, min.y, max.y) };
		}

		/*-------------------------------------------------------------------------------------
		| --- Reflect: Reflects a vector off the plane (line) defined by the given normal --- |
		-------------------------------------------------------------------------------------*/
		static constexpr inline Vector2 Reflect(const Vector2& inDirection, const Vector2& inNormal)
		{
			return inDirection - inNormal * (2 * Dot(inDirection, inNormal));
		}

		/*---------------------------------------------------------------
		| --- MoveTowards: Moves a point 'current' towards 'target' --- |
		---------------------------------------------------------------*/
		static inline Vector2 MoveTowards(const Vector2& current, const Vector2& target, const T maxDistanceDelta)
		{
			Vector2 delta = target - current;
			T distance = delta.Magnitude();

			if (distance <= maxDistanceDelta || distance == static_cast<T>(0))
			{
				return target;
			}

			return current + delta / distance * maxDistanceDelta;
		}

		/*----------------------------------------------------------
		| --- Lerp: Linearly interpolates between a and b by t --- |
		----------------------------------------------------------*/
		static constexpr inline Vector2 Lerp(const Vector2& a, const Vector2& b, const float t)
		{
			float clampedT = std::clamp(t, 0.0f, 1.0f);
			return a + (b - a) * clampedT;
		}

		/*----------------------------------------------------------------------------------------------------
		| --- LerpUnclamped: Linearly interpolates between a and b by t without clamping the interpolant --- |
		----------------------------------------------------------------------------------------------------*/
		static constexpr inline Vector2 LerpUnclamped(const Vector2& a, const Vector2& b, const float t)
		{
			return a + (b - a) * t;
		}
	};

	// Type aliases for common vector types
	using Vector2i = Vector2<int>;
	using Vector2f = Vector2<float>;
	using Vector2d = Vector2<double>;
}