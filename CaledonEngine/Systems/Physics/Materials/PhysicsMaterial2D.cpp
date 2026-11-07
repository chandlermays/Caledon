/*------------------------------
| File: PhysicsMaterial2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "PhysicsMaterial2D.h"

#include <algorithm>
#include <cmath>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------------------
| --- Constructor: Constructs the PhysicsMaterial2D with default surface data --- |
---------------------------------------------------------------------------------*/
CaledonEngine::PhysicsMaterial2D::PhysicsMaterial2D()
	: m_friction{ 0.6f }
	, m_bounciness{ 0.0f }
	, m_frictionCombine{ PhysicsMaterialCombine2D::Mean }
	, m_bounceCombine{ PhysicsMaterialCombine2D::Maximum }
{ }

/*----------------------------------------------------------------------------------------------
| --- Constructor: Constructs the PhysicsMaterial2D with the given friction and bounciness --- |
----------------------------------------------------------------------------------------------*/
CaledonEngine::PhysicsMaterial2D::PhysicsMaterial2D(float friction, float bounciness)
	: m_friction{ friction }
	, m_bounciness{ bounciness }
	, m_frictionCombine{ PhysicsMaterialCombine2D::Mean }
	, m_bounceCombine{ PhysicsMaterialCombine2D::Maximum }
{ }

/*------------------------------------------------------------------------------------------------------------
| --- GetCombinedValues: Returns the effective friction or bounciness value used in a collision response --- |
------------------------------------------------------------------------------------------------------------*/
float CaledonEngine::PhysicsMaterial2D::GetCombinedValues(float valueA, float valueB, PhysicsMaterialCombine2D combineA, PhysicsMaterialCombine2D combineB)
{
	const PhysicsMaterialCombine2D combine = std::max(combineA, combineB);

	switch (combine)
	{
	case PhysicsMaterialCombine2D::Average:
		return (valueA + valueB) * 0.5f;

	case PhysicsMaterialCombine2D::Mean:
		return std::sqrt(valueA * valueB);

	case PhysicsMaterialCombine2D::Multiply:
		return valueA * valueB;

	case PhysicsMaterialCombine2D::Minimum:
		return std::min(valueA, valueB);

	case PhysicsMaterialCombine2D::Maximum:
	default:
		return std::max(valueA, valueB);
	}
}