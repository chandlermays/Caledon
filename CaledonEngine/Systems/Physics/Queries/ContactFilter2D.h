/*------------------------------
| File: ContactFilter2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Physics/LayerMask.h"
#include "Utilities/Math/Constants.h"
#include "Utilities/Math/Vector2.h"

#include <cmath>
#include <limits>

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
		static constexpr float kNormalAngleUpperLimit = 359.9999f;							//

		LayerMask layerMask{ -1 };															//

		float maxDepth = std::numeric_limits<float>::infinity();							//
		float minDepth = -std::numeric_limits<float>::infinity();							//

		float maxNormalAngle = kNormalAngleUpperLimit;										//
		float minNormalAngle = 0.0f;														//

		bool useDepth = false;																//
		bool useLayerMask = false;															//
		bool useNormalAngle = false;														//
		bool useOutsideDepth = false;														//
		bool useOutsideNormalAngle = false;													//
		bool useTriggers = true;															//

		/*---------------------------------------------------------------------------------------
		| --- NoFilter:
		---------------------------------------------------------------------------------------*/
		static ContactFilter2D NoFilter() noexcept
		{
			return ContactFilter2D{ };
		}

		/*---------------------------------------------------------------------------------------
		| --- IsFiltering:
		---------------------------------------------------------------------------------------*/
		bool IsFiltering() const noexcept
		{
			return !useTriggers || useLayerMask || useDepth || useNormalAngle;
		}

		/*---------------------------------------------------------------------------------------
		| --- ClearDepth:
		---------------------------------------------------------------------------------------*/
		void ClearDepth() noexcept
		{
			useDepth = false;
		}

		/*---------------------------------------------------------------------------------------
		| --- ClearLayerMask:
		---------------------------------------------------------------------------------------*/
		void ClearLayerMask() noexcept
		{
			useLayerMask = false;
		}

		/*---------------------------------------------------------------------------------------
		| --- ClearNormalAngle:
		---------------------------------------------------------------------------------------*/
		void ClearNormalAngle() noexcept
		{
			useNormalAngle = false;
		}

		/*---------------------------------------------------------------------------------------
		| --- SetDepth:
		---------------------------------------------------------------------------------------*/
		void SetDepth(float min, float max) noexcept
		{
			minDepth = min;
			maxDepth = max;
			useDepth = true;
		}

		/*---------------------------------------------------------------------------------------
		| --- SetLayerMask:
		---------------------------------------------------------------------------------------*/
		void SetLayerMask(LayerMask mask) noexcept
		{
			layerMask = mask;
			useLayerMask = true;
		}

		/*---------------------------------------------------------------------------------------
		| --- SetNormalAngle:
		---------------------------------------------------------------------------------------*/
		void SetNormalAngle(float min, float max) noexcept
		{
			minNormalAngle = min;
			maxNormalAngle = max;
			useNormalAngle = true;
		}

		/*---------------------------------------------------------------------------------------
		| --- IsFilteringDepth:
		---------------------------------------------------------------------------------------*/
		bool IsFilteringDepth(float depth) const noexcept
		{
			if (!useDepth)
				return false;

			const bool inRange = depth >= minDepth && depth <= maxDepth;
			return useOutsideDepth ? inRange : !inRange;
		}

		/*---------------------------------------------------------------------------------------
		| --- IsFilteringLayerMask:
		---------------------------------------------------------------------------------------*/
		bool IsFilteringLayerMask(int layer) const noexcept
		{
			if (!useLayerMask)
				return false;

			if (layer < 0 || layer > 31)
				return true;

			return (static_cast<unsigned int>(layerMask.value) & (1u << layer)) == 0u;
		}

		/*---------------------------------------------------------------------------------------
		| --- IsFilteringNormalAngle:
		---------------------------------------------------------------------------------------*/
		bool IsFilteringNormalAngle(float angle) const noexcept
		{
			if (!useNormalAngle)
				return false;

			angle -= std::floor(angle / 360.0f) * 360.0f;

			const bool inRange = angle >= minNormalAngle && angle <= maxNormalAngle;
			return useOutsideNormalAngle ? inRange : !inRange;
		}

		/*---------------------------------------------------------------------------------------
		| --- IsFilteringNormalAngle:
		---------------------------------------------------------------------------------------*/
		bool IsFilteringNormalAngle(const Vector2f& normal) const noexcept
		{
			return IsFilteringNormalAngle(std::atan2(normal.y, normal.x) * Math::RAD2DEG);
		}

		/*---------------------------------------------------------------------------------------
		| --- IsFilteringTrigger:
		---------------------------------------------------------------------------------------*/
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