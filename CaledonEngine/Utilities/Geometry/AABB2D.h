/*------------------------------
| File: AABB2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Utilities/Math/Vector2.h"

#include <limits>

namespace CaledonEngine
{
	struct AABB2D
	{
		Vector2f min;																									// The minimum point of the bounding box
		Vector2f max;																									// The maximum point of the bounding box

		constexpr AABB2D()																								// Default constructor initializes the AABB to an invalid state
			: min{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() }
			, max{ std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() }
		{}

		constexpr AABB2D(const Vector2f& minPoint, const Vector2f& maxPoint)											// Constructs an AABB from a minimum and maximum point
			: min{ minPoint }
			, max{ maxPoint }
		{}

		constexpr AABB2D(const Vector2f& center, const float radius)													// Constructs an AABB from a center point and a radius
			: min(center - Vector2f(radius))
			, max(center + Vector2f(radius))
		{}

		constexpr bool operator==(const AABB2D& other) const { return min == other.min && max == other.max; }			// Compare two AABBs for equality
		constexpr bool operator!=(const AABB2D& other) const { return !(*this == other); }								// Compare two AABBs for inequality

		constexpr Vector2f GetCenter() const { return (min + max) / 2.0f; }												// Returns the center point of the AABB
		constexpr Vector2f GetSize() const { return max - min; }														// Returns the size of the AABB
		constexpr Vector2f GetExtents() const { return GetSize() / 2.0f; }												// Returns the extents (half-size) of the AABB
		constexpr float GetArea() const { Vector2f size = GetSize(); return size.x * size.y; }							// Returns the area of the AABB

		constexpr bool IsValid() const { return min.x <= max.x && min.y <= max.y; }										// Returns true if the AABB is valid (min <= max)

		/*----------------------------------------------------
		| --- Reset: Resets the AABB to an invalid state --- |
		----------------------------------------------------*/
		constexpr void Reset()
		{
			min = Vector2f{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
			max = Vector2f{ std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };
		}

		/*-------------------------------------------------------------------
		| --- Encapsulate: Expands this AABB to contain the given point --- |
		-------------------------------------------------------------------*/
		constexpr void Encapsulate(const Vector2f& point)
		{
			min = Vector2f::Min(min, point);
			max = Vector2f::Max(max, point);
		}

		/*------------------------------------------------------------------
		| --- Encapsulate: Expands this AABB to contain the given AABB --- |
		------------------------------------------------------------------*/
		constexpr void Encapsulate(const AABB2D& box)
		{
			Encapsulate(box.min);
			Encapsulate(box.max);
		}

		/*-----------------------------------------------------------------------------
		| --- Intersect: Returns a new AABB that is the intersection of two AABBs --- |
		-----------------------------------------------------------------------------*/
		constexpr AABB2D Intersect(const AABB2D& other) const
		{
			return AABB2D(Vector2f::Max(min, other.min), Vector2f::Min(max, other.max));
		}

		/*----------------------------------------------------------
		| --- ExpandBy: Expands the AABB by the given distance --- |
		----------------------------------------------------------*/
		constexpr void ExpandBy(const Vector2f& distance)
		{
			min -= distance;
			max += distance;
		}

		/*--------------------------------------------------------------------
		| --- Contains: Returns true if the other box is inside this box --- |
		--------------------------------------------------------------------*/
		constexpr bool Contains(const AABB2D& other) const
		{
			return other.min.x >= min.x && other.max.x <= max.x &&
				other.min.y >= min.y && other.max.y <= max.y;
		}

		/*----------------------------------------------------------------
		| --- Contains: Returns true if the point is inside this box --- |
		----------------------------------------------------------------*/
		constexpr bool Contains(const Vector2f& point) const
		{
			return point.x >= min.x && point.x <= max.x &&
				point.y >= min.y && point.y <= max.y;
		}

		/*-------------------------------------------------------
		| --- Overlaps: Returns true if two AABBs intersect --- |
		-------------------------------------------------------*/
		constexpr bool Overlaps(const AABB2D& other) const
		{
			return !(max.x < other.min.x || min.x > other.max.x ||
				max.y < other.min.y || min.y > other.max.y);
		}

		/*------------------------------------------------------------
		| --- Translate: Moves the AABB by the given translation --- |
		------------------------------------------------------------*/
		constexpr void Translate(const Vector2f& translation)
		{
			min += translation;
			max += translation;
		}

		/*---------------------------------------------------
		| --- Scale: Returns a scaled copy of this AABB --- |
		---------------------------------------------------*/
		constexpr AABB2D Scale(const Vector2f& scale) const
		{
			Vector2f center = GetCenter();
			Vector2f extents = GetExtents() * scale;
			return AABB2D(center - extents, center + extents);
		}

		/*-----------------------------------------------------------------------------
		| --- GetClosestPoint: Returns the closest point in the AABB to the point --- |
		-----------------------------------------------------------------------------*/
		constexpr Vector2f GetClosestPoint(const Vector2f& point) const
		{
			return Vector2f::Clamp(point, min, max);
		}

		/*---------------------------------------------------------------------
		| --- DistanceTo: Returns the distance from the AABB to the point --- |
		---------------------------------------------------------------------*/
		float DistanceTo(const Vector2f& point) const
		{
			return Vector2f::Distance(point, GetClosestPoint(point));
		}

		/*--------------------------------------------------------------------------------
		| --- DistanceToSqr: Returns the squared distance from the AABB to the point --- |
		--------------------------------------------------------------------------------*/
		float DistanceToSqr(const Vector2f& point) const
		{
			return Vector2f::SqrDistance(point, GetClosestPoint(point));
		}
	};
}
