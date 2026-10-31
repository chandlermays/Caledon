/*------------------------------
| File: ShapeManifold2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/Shapes/ShapeQueries2D.h"
#include "Utilities/Math/Vector2.h"

// Pure contact geometry for two shapes that are already in world space (see Collider2D::GetShapes)
//
// Given two touching or overlapping shapes, Collide() answers "where do they touch, which way do they push apart, and how deep are they?"
// This is what the solver needs to push bodies apart, and what the contact list and collision callbacks report.
// It knows nothing about colliders, bodies or scenes
//
// Supported shape pairs (anything else is reported as no contact, see ShapeQueries2D::IsSupported):
// * Circle  - Circle
// * Polygon - Circle
// * Polygon - Polygon (convex, counter-clockwise; the radius rounds the corners, like in ShapeQueries2D)
//
// Conventions
// * The normal is a unit vector that points from shape A toward shape B. To separate the shapes, move B along +normal (or A along -normal)
// * Separation is the gap between the two surfaces along the normal: negative = overlapping by that much, 0 = just touching. Only contacts with separation <= 0 are reported
// * A contact point lies halfway between the two surfaces, along the normal
// * A manifold has one point (curved or corner contacts) or two points (a flat face resting on a flat face)

namespace CaledonEngine
{
	struct ShapeManifoldPoint2D
	{
		Vector2f point{ };																							// The contact point in world space (halfway between the two surfaces)
		float separation = 0.0f;																					// The gap along the normal (negative when the shapes overlap)
	};

	struct ShapeManifold2D
	{
		Vector2f normal{ };																							// The unit normal pointing from shape A toward shape B
		int pointCount = 0;																							// How many entries of 'points' are valid (0, 1 or 2)
		ShapeManifoldPoint2D points[2];																			// The contact points

		void Clear() noexcept																						// Empties the manifold
		{
			normal = Vector2f{ 0.0f, 0.0f };
			pointCount = 0;
			points[0] = ShapeManifoldPoint2D{ };
			points[1] = ShapeManifoldPoint2D{ };
		}
	};

	namespace ShapeManifold2DQueries
	{
		bool Collide(const ShapeView2D& shapeA, const ShapeView2D& shapeB, ShapeManifold2D& manifold);				// Computes the contact between two shapes; returns false (and an empty manifold) if they don't touch or aren't supported
	}
}
