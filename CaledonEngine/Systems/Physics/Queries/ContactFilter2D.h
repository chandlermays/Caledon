/*------------------------------
| File: ContactFilter2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/LayerMask.h"
#include "Utilities/Math/Constants.h"
#include "Utilities/Math/Vector2.h"

#include <cmath>

// Filters contact results by angle, layer, depth, and trigger state.
// Angles are measured in world space, with 0° along +X and increasing counterclockwise.
//
// A default-constructed filter is equivalent to NoFilter().
// IsFiltering*() returns true if a value would be excluded, false if kept.
// Disabled filter options always return false, so their use* flags need not be checked.
// Filtering operates on raw values, keeping the filter independent of scene objects.
// Scene-facing overloads can be added when the required GameObject and Collider2D APIs exist.

namespace CaledonEngine
{
	struct ContactFilter2D
	{
		static constexpr float kNormalAngleUpperLimit = 359.9999f;							// The maximum angle allowed for a normal angle filter

		LayerMask layerMask{ -1 };															// The layer mask used to filter contacts by layer

		float maxDepth = Math::Infinity;													// The maximum depth allowed for a depth filter
		float minDepth = -Math::Infinity;													// The minimum depth allowed for a depth filter

		float maxNormalAngle = kNormalAngleUpperLimit;										// The maximum angle allowed for a normal angle filter
		float minNormalAngle = 0.0f;														// The minimum angle allowed for a normal angle filter

		bool useDepth = false;																// Indicates whether to filter contacts by depth
		bool useLayerMask = false;															// Indicates whether to filter contacts by layer
		bool useNormalAngle = false;														// Indicates whether to filter contacts by normal angle
		bool useOutsideDepth = false;														// Indicates whether to filter contacts outside (or inside) the depth range
		bool useOutsideNormalAngle = false;													// Indicates whether to filter contacts outside (or inside) the normal angle range
		bool useTriggers = true;															// Indicates whether to include trigger colliders

		/*-------------------------------------------------------------------------------
		| --- NoFilter: Returns a ContactFilter2D that does not filter any contacts --- |
		-------------------------------------------------------------------------------*/
		static ContactFilter2D NoFilter() noexcept
		{
			return ContactFilter2D{ };
		}

		/*----------------------------------------------------------------------------------------------------------------
		| --- CreateLegacyFilter: Returns a ContactFilter2D that filters contacts by layer, depth, and trigger state --- |
		----------------------------------------------------------------------------------------------------------------*/
		static ContactFilter2D CreateLegacyFilter(int layerMask, float minDepth, float maxDepth, bool useTriggers = true) noexcept
		{
			ContactFilter2D contactFilter;
			contactFilter.SetLayerMask(LayerMask{ layerMask });

			if (minDepth != -Math::Infinity || maxDepth != Math::Infinity)
			{
				contactFilter.SetDepth(minDepth, maxDepth);
			}

			contactFilter.useTriggers = useTriggers;
			return contactFilter;
		}

		/*-------------------------------------------------------------------------------------
		| --- IsFiltering: Returns whether this ContactFilter2D is filtering any contacts --- |
		-------------------------------------------------------------------------------------*/
		bool IsFiltering() const noexcept
		{
			return !useTriggers || useLayerMask || useDepth || useNormalAngle;
		}

		/*----------------------------------------------------------------------
		| --- ClearDepth: Clears the depth filter for this ContactFilter2D --- |
		----------------------------------------------------------------------*/
		void ClearDepth() noexcept
		{
			useDepth = false;
		}

		/*-------------------------------------------------------------------------------
		| --- ClearLayerMask: Clears the layer mask filter for this ContactFilter2D --- |
		-------------------------------------------------------------------------------*/
		void ClearLayerMask() noexcept
		{
			useLayerMask = false;
		}

		/*-----------------------------------------------------------------------------------
		| --- ClearNormalAngle: Clears the normal angle filter for this ContactFilter2D --- |
		-----------------------------------------------------------------------------------*/
		void ClearNormalAngle() noexcept
		{
			useNormalAngle = false;
		}

		/*-------------------------------------------------------------------------------------------
		| --- SetDepth: Sets the depth filter for this ContactFilter2D, with min and max values --- |
		-------------------------------------------------------------------------------------------*/
		void SetDepth(float min, float max) noexcept
		{
			minDepth = min;
			maxDepth = max;
			useDepth = true;
		}

		/*---------------------------------------------------------------------------
		| --- SetLayerMask: Sets the layer mask filter for this ContactFilter2D --- |
		---------------------------------------------------------------------------*/
		void SetLayerMask(LayerMask mask) noexcept
		{
			layerMask = mask;
			useLayerMask = true;
		}

		/*--------------------------------------------------------------------------------------------------------
		| --- SetNormalAngle: Sets the normal angle filter for this ContactFilter2D, with min and max values --- |
		--------------------------------------------------------------------------------------------------------*/
		void SetNormalAngle(float min, float max) noexcept
		{
			minNormalAngle = min;
			maxNormalAngle = max;
			useNormalAngle = true;
		}

		/*-----------------------------------------------------------------------------------------------
		| --- IsFilteringDepth: Returns whether this ContactFilter2D is filtering contacts by depth --- |
		-----------------------------------------------------------------------------------------------*/
		bool IsFilteringDepth(float depth) const noexcept
		{
			if (!useDepth)
				return false;

			const bool inRange = depth >= minDepth && depth <= maxDepth;
			return useOutsideDepth ? inRange : !inRange;
		}

		/*---------------------------------------------------------------------------------------------------
		| --- IsFilteringLayerMask: Returns whether this ContactFilter2D is filtering contacts by layer --- |
		---------------------------------------------------------------------------------------------------*/
		bool IsFilteringLayerMask(int layer) const noexcept
		{
			if (!useLayerMask)
				return false;

			if (layer < 0 || layer > 31)
				return true;

			return (static_cast<unsigned int>(layerMask.value) & (1u << layer)) == 0u;
		}

		/*------------------------------------------------------------------------------------------------------------
		| --- IsFilteringNormalAngle: Returns whether this ContactFilter2D is filtering contacts by normal angle --- |
		------------------------------------------------------------------------------------------------------------*/
		bool IsFilteringNormalAngle(float angle) const noexcept
		{
			if (!useNormalAngle)
				return false;

			angle -= std::floor(angle / 360.0f) * 360.0f;

			const bool inRange = angle >= minNormalAngle && angle <= maxNormalAngle;
			return useOutsideNormalAngle ? inRange : !inRange;
		}

		/*------------------------------------------------------------------------------------------------------------
		| --- IsFilteringNormalAngle: Returns whether this ContactFilter2D is filtering contacts by normal angle --- |
		------------------------------------------------------------------------------------------------------------*/
		bool IsFilteringNormalAngle(const Vector2f& normal) const noexcept
		{
			return IsFilteringNormalAngle(std::atan2(normal.y, normal.x) * Math::Rad2Deg);
		}

		/*---------------------------------------------------------------------------------------------------------
		| --- IsFilteringTrigger: Returns whether this ContactFilter2D is filtering contacts by trigger state --- |
		---------------------------------------------------------------------------------------------------------*/
		bool IsFilteringTrigger(bool isTrigger) const noexcept
		{
			return isTrigger && !useTriggers;
		}

		// To add once the scene side exists (each would just forward to the raw-value overloads above):
		//		bool IsFilteringDepth(const GameObject& obj) const				// needs a depth source: Transform is 2D (Vector2f) and has no Z
		//		bool IsFilteringLayerMask(const GameObject& obj) const			// needs GameObject::GetLayer()
		//		bool IsFilteringTrigger(const Collider2D& collider) const		// needs Collider2D::IsTrigger()
	};
}