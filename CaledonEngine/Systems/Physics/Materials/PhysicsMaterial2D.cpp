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
{}

/*----------------------------------------------------------------------------------------------
| --- Constructor: Constructs the PhysicsMaterial2D with the given friction and bounciness --- |
----------------------------------------------------------------------------------------------*/
CaledonEngine::PhysicsMaterial2D::PhysicsMaterial2D(float friction, float bounciness)
	: m_friction{ friction }
	, m_bounciness{ bounciness }
	, m_frictionCombine{ PhysicsMaterialCombine2D::Mean }
	, m_bounceCombine{ PhysicsMaterialCombine2D::Maximum }
{}

/*----------------------------------------------------------
| --- GetFriction: Returns the coefficient of friction --- |
----------------------------------------------------------*/
float CaledonEngine::PhysicsMaterial2D::GetFriction() const
{
	return m_friction;
}

/*-------------------------------------------------------
| --- SetFriction: Sets the coefficient of friction --- |
-------------------------------------------------------*/
void CaledonEngine::PhysicsMaterial2D::SetFriction(float friction)
{
	m_friction = friction;
}

/*---------------------------------------------------------------
| --- GetBounciness: Returns the coefficient of restitution --- |
---------------------------------------------------------------*/
float CaledonEngine::PhysicsMaterial2D::GetBounciness() const
{
	return m_bounciness;
}

/*------------------------------------------------------------
| --- SetBounciness: Sets the coefficient of restitution --- |
------------------------------------------------------------*/
void CaledonEngine::PhysicsMaterial2D::SetBounciness(float bounciness)
{
	m_bounciness = bounciness;
}

/*-----------------------------------------------------------------------------------------------
| --- GetFrictionCombine: Returns the combine mode used when calculating effective friction --- |
-----------------------------------------------------------------------------------------------*/
CaledonEngine::PhysicsMaterialCombine2D CaledonEngine::PhysicsMaterial2D::GetFrictionCombine() const
{
	return m_frictionCombine;
}

/*--------------------------------------------------------------------------------------------
| --- SetFrictionCombine: Sets the combine mode used when calculating effective friction --- |
--------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsMaterial2D::SetFrictionCombine(PhysicsMaterialCombine2D combine)
{
	m_frictionCombine = combine;
}

/*-----------------------------------------------------------------------------------------------
| --- GetBounceCombine: Returns the combine mode used when calculating effective bounciness --- |
-----------------------------------------------------------------------------------------------*/
CaledonEngine::PhysicsMaterialCombine2D CaledonEngine::PhysicsMaterial2D::GetBounceCombine() const
{
	return m_bounceCombine;
}

/*--------------------------------------------------------------------------------------------
| --- SetBounceCombine: Sets the combine mode used when calculating effective bounciness --- |
--------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsMaterial2D::SetBounceCombine(PhysicsMaterialCombine2D combine)
{
	m_bounceCombine = combine;
}

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