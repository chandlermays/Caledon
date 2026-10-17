/*------------------------------
| File: ColliderDistance2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Math/Vector2.h"

// Represents the separation or overlap of two Collider2D
//
// This struct primarily defines a point on the exterior of each Collider2D along with the distance between those two points.
// The distance between them can be positive indicating that the Collider2D are separated (not overlapping),
// zero indicating that they are touching (but not overlapped), or negative indicating that they are overlapped

namespace CaledonEngine
{
	struct ColliderDistance2D
	{
		Vector2f pointA;						// A point on a Collider2D that is a specific distance away from pointB
		Vector2f pointB;						// A point on a Collider2D that is a specific distance away from pointA
		Vector2f normal;						// A normalized vector that points from pointB to pointA

		float distance;							// Returns the distance between two Collider2D
		bool isOverlapped;						// Checks whether the distance represents an overlap or not
		bool isValid;							// Checks whether the distance is valid or not

		/*----------------------------------------------------------------------------
		| --- Constructor: Constructs the ColliderDistance2D with default values --- |
		----------------------------------------------------------------------------*/
		constexpr ColliderDistance2D()
			: pointA{}
			, pointB{}
			, normal{}
			, distance{ 0.0f }
			, isOverlapped{ false }
			, isValid{ false }
		{}

		/*----------------------------------------------------------------------------------
		| --- Constructor: Constructs the ColliderDistance2D with paramaterized values --- |
		----------------------------------------------------------------------------------*/
		constexpr ColliderDistance2D(const Vector2f& pointA, const Vector2f& pointB, const Vector2f& normal, float distance, bool isOverlapped)
			: pointA{ pointA }
			, pointB{ pointB }
			, normal{ normal }
			, distance{ distance }
			, isOverlapped{ isOverlapped }
			, isValid{ true }
		{}

		/*------------------------------------------------------------------
		| --- operator==: Compares two ColliderDistance2D for equality --- |
		------------------------------------------------------------------*/
		constexpr bool operator==(const ColliderDistance2D& other) const
		{
			return pointA == other.pointA && pointB == other.pointB && normal == other.normal &&
				distance == other.distance && isOverlapped == other.isOverlapped && isValid == other.isValid;
		}

		/*--------------------------------------------------------------------
		| --- operator!=: Compares two ColliderDistance2D for inequality --- |
		--------------------------------------------------------------------*/
		constexpr bool operator!=(const ColliderDistance2D& other) const
		{
			return !(*this == other);
		}
	};
}