/*------------------------------
| File: SlideConfig2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/LayerMask.h"
#include "Utilities/Math/Vector2.h"

// The configuration that controls how RigidBody2D::Slide behaves

namespace CaledonEngine
{
	class Collider2D;

	struct SlideConfig2D
	{
		Vector2f gravity;								// The gravity to be applied to the slide position
		float gravitySlipAngle;							// When gravity movement causes a contact with a Collider2D, slippage may occur if the surface angle is greater than this angle
		LayerMask layerMask;							// The layer mask used for filtering colliders
		int maxIterations;								// Controls the maximum number of iterations to perform when determining how a RigidBody2D will slide
		Collider2D* selectedCollider;					// The specific Collider2D attached to this RigidBody2D used to detect contacts
		Vector2f startPosition;							// The start position to slide the RigidBody2D from
		Vector2f surfaceAnchor;							// Direction and distance to check for nearby surfaces during a slide
		float surfaceSlideAngle;						// Maximum slope angle for sliding
		Vector2f surfaceUp;								// Reference up vector for slope/angle calculations
		bool useAttachedTriggers;						// Whether trigger colliders on the body are used
		bool useLayerMask;								// Whether layerMask is applied
		bool useNoMove;									// Controls whether movement actually occurs
		bool useSimulationMove;							// Whether movement uses MovePosition instead of instant positioning
		bool useStartPosition;							// Whether startPosition is used as the origin

		/*-----------------------------------------------------------------------
		| --- Constructor: Constructs the SlideConfig2D with default values --- |
		-----------------------------------------------------------------------*/
		constexpr SlideConfig2D()
			: gravity{}
			, gravitySlipAngle{ 90.0f }
			, layerMask{ -1 }
			, maxIterations{ 3 }
			, selectedCollider{ nullptr }
			, startPosition{}
			, surfaceAnchor{ 0.0f, -0.01f }
			, surfaceSlideAngle{ 90.0f }
			, surfaceUp{ 0.0f, 1.0f }
			, useAttachedTriggers{ false }
			, useLayerMask{ false }
			, useNoMove{ false }
			, useSimulationMove{ false }
			, useStartPosition{ false }
		{ }

		/*-----------------------------------------------------------------
		| --- SetLayerMask: Sets the layer mask to this SlideConfig2D --- |
		-----------------------------------------------------------------*/
		void SetLayerMask(LayerMask mask) noexcept
		{
			layerMask = mask;
			useLayerMask = true;
		}

		/*-------------------------------------------------------------------------
		| --- SetStartPosition: Sets the start position of this SlideConfig2D --- |
		-------------------------------------------------------------------------*/
		void SetStartPosition(const Vector2f& position) noexcept
		{
			startPosition = position;
			useStartPosition = true;
		}
	};
}