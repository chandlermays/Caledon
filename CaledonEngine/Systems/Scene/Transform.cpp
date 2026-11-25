/*------------------------------
| File: Transform.cpp
| Author: Chandler Mays
------------------------------*/
#include "Transform.h"

#include "Systems/Scene/GameObject.h"
#include "Utilities/Math/Constants.h"

#include <cmath>

namespace
{
	using CaledonEngine::Vector2f;

	/*--------------------------------------------------------------------------------------------------
	| --- IsMirrored: Returns whether a scale flips space (exactly one negative axis), which turns --- |
	| ---             rotations the other way                                                      --- |
	--------------------------------------------------------------------------------------------------*/
	bool IsMirrored(const Vector2f& scale)
	{
		return (scale.x < 0.0f) != (scale.y < 0.0f);
	}

	/*---------------------------------------------------------------------------------------------------
	| --- SafeDivide: Divides by a scale axis, keeping the fallback when the axis is (nearly) zero, --- |
	| ---             since a zero scale squashes every value onto one point and can't be undone    --- |
	---------------------------------------------------------------------------------------------------*/
	float SafeDivide(float value, float divisor, float fallback)
	{
		return (std::abs(divisor) > CaledonEngine::Math::Epsilon) ? value / divisor : fallback;
	}
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------
| --- Constructor: Constructs the Transform with default values --- |
-------------------------------------------------------------------*/
CaledonEngine::Transform::Transform()
	: Transform{ Vector2f{ 0.0f, 0.0f }, 0.0f, Vector2f{ 1.0f, 1.0f } }
{
}

/*-----------------------------------------------------------------------------------------
| --- Parameterized Constructor: Constructs the Transform with the given local values --- |
-----------------------------------------------------------------------------------------*/
CaledonEngine::Transform::Transform(const Vector2f& localPosition, float localRotation, const Vector2f& localScale)
	: Component()
	, m_localPosition{ localPosition }
	, m_localScale{ localScale }
	, m_localRotation{ localRotation }
	, m_worldPosition{ localPosition }
	, m_worldScale{ localScale }
	, m_worldRotation{ localRotation }
	, m_worldLinear{ 1.0f, 0.0f, 0.0f, 1.0f }
	, m_isWorldDirty{ true }
{
}

/*-------------------------------------------------------------
| --- GetTypeName: Returns the type name of the Component --- |
-------------------------------------------------------------*/
const std::string& CaledonEngine::Transform::GetTypeName() const
{
	static const std::string typeName = "Transform";
	return typeName;
}

/*---------------------------------------------------------------
| --- GetPosition: Returns the world position of the object --- |
---------------------------------------------------------------*/
const CaledonEngine::Vector2f& CaledonEngine::Transform::GetPosition() const
{
	UpdateWorld();
	return m_worldPosition;
}

/*-----------------------------------------------------------------------------------------------
| --- SetPosition: Sets the world position of the object (stored as the matching local one) --- |
-----------------------------------------------------------------------------------------------*/
void CaledonEngine::Transform::SetPosition(const Vector2f& position)
{
	const Transform* pParent = GetParentTransform();
	if (!pParent)
	{
		SetLocalPosition(position);
		return;
	}

	// local = W^-1 * (world - parentPosition), with W the parent's world 2x2 matrix [a c; b d]
	pParent->UpdateWorld();
	const float* w = pParent->m_worldLinear;
	const float determinant = w[0] * w[3] - w[2] * w[1];
	if (std::abs(determinant) <= CaledonEngine::Math::Epsilon)
		return;																					// A zero-scale parent squashes everything onto one point; keep the current local position

	const Vector2f offset = position - pParent->m_worldPosition;
	SetLocalPosition(Vector2f{
		(w[3] * offset.x - w[2] * offset.y) / determinant,
		(-w[1] * offset.x + w[0] * offset.y) / determinant });
}

/*---------------------------------------------------------------------------
| --- GetRotation: Returns the world rotation of the object, in degrees --- |
---------------------------------------------------------------------------*/
float CaledonEngine::Transform::GetRotation() const
{
	UpdateWorld();
	return m_worldRotation;
}

/*------------------------------------------------------------------------
| --- SetRotation: Sets the world rotation of the object, in degrees --- |
------------------------------------------------------------------------*/
void CaledonEngine::Transform::SetRotation(float rotation)
{
	const Transform* pParent = GetParentTransform();
	if (!pParent)
	{
		SetLocalRotation(rotation);
		return;
	}

	const float parentRotation = pParent->GetRotation();
	SetLocalRotation(IsMirrored(pParent->GetScale()) ? parentRotation - rotation : rotation - parentRotation);
}

/*-----------------------------------------------------------------
| --- GetScale: Returns the world (lossy) scale of the object --- |
-----------------------------------------------------------------*/
const CaledonEngine::Vector2f& CaledonEngine::Transform::GetScale() const
{
	UpdateWorld();
	return m_worldScale;
}

/*------------------------------------------------------
| --- SetScale: Sets the world scale of the object --- |
------------------------------------------------------*/
void CaledonEngine::Transform::SetScale(const Vector2f& scale)
{
	const Transform* pParent = GetParentTransform();
	if (!pParent)
	{
		SetLocalScale(scale);
		return;
	}

	const Vector2f& parentScale = pParent->GetScale();
	SetLocalScale(Vector2f{
		SafeDivide(scale.x, parentScale.x, m_localScale.x),
		SafeDivide(scale.y, parentScale.y, m_localScale.y) });
}

/*-----------------------------------------------------------------------
| --- GetLocalPosition: Returns the position relative to the parent --- |
-----------------------------------------------------------------------*/
const CaledonEngine::Vector2f& CaledonEngine::Transform::GetLocalPosition() const noexcept
{
	return m_localPosition;
}

/*--------------------------------------------------------------------
| --- SetLocalPosition: Sets the position relative to the parent --- |
--------------------------------------------------------------------*/
void CaledonEngine::Transform::SetLocalPosition(const Vector2f& localPosition)
{
	m_localPosition = localPosition;
	MarkWorldDirty();
}

/*-----------------------------------------------------------------------------------
| --- GetLocalRotation: Returns the rotation relative to the parent, in degrees --- |
-----------------------------------------------------------------------------------*/
float CaledonEngine::Transform::GetLocalRotation() const noexcept
{
	return m_localRotation;
}

/*--------------------------------------------------------------------------------
| --- SetLocalRotation: Sets the rotation relative to the parent, in degrees --- |
--------------------------------------------------------------------------------*/
void CaledonEngine::Transform::SetLocalRotation(float localRotation)
{
	m_localRotation = localRotation;
	MarkWorldDirty();
}

/*-----------------------------------------------------------------
| --- GetLocalScale: Returns the scale relative to the parent --- |
-----------------------------------------------------------------*/
const CaledonEngine::Vector2f& CaledonEngine::Transform::GetLocalScale() const noexcept
{
	return m_localScale;
}

/*--------------------------------------------------------------
| --- SetLocalScale: Sets the scale relative to the parent --- |
--------------------------------------------------------------*/
void CaledonEngine::Transform::SetLocalScale(const Vector2f& localScale)
{
	m_localScale = localScale;
	MarkWorldDirty();
}

/*----------------------------------------------------------------------------------------
| --- TransformPoint: Converts a point from this object's local space to world space --- |
----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Transform::TransformPoint(const Vector2f& localPoint) const
{
	UpdateWorld();
	const float* w = m_worldLinear;
	return m_worldPosition + Vector2f{ w[0] * localPoint.x + w[2] * localPoint.y, w[1] * localPoint.x + w[3] * localPoint.y };
}

/*-----------------------------------------------------------------------------------------------
| --- InverseTransformPoint: Converts a point from world space to this object's local space --- |
-----------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Transform::InverseTransformPoint(const Vector2f& worldPoint) const
{
	UpdateWorld();
	const float* w = m_worldLinear;
	const float determinant = w[0] * w[3] - w[2] * w[1];
	if (std::abs(determinant) <= CaledonEngine::Math::Epsilon)
		return Vector2f{ 0.0f, 0.0f };

	const Vector2f offset = worldPoint - m_worldPosition;
	return Vector2f{ (w[3] * offset.x - w[2] * offset.y) / determinant, (-w[1] * offset.x + w[0] * offset.y) / determinant };
}

/*-----------------------------------------------------------------------------------------------------
| --- MarkWorldDirty: Flags the cached world pose (and every child's) to be recalculated. A dirty --- |
| ---                 Transform always has dirty children, so an already dirty one can stop here  --- |
-----------------------------------------------------------------------------------------------------*/
void CaledonEngine::Transform::MarkWorldDirty() const
{
	if (m_isWorldDirty)
		return;

	m_isWorldDirty = true;

	if (!m_pOwner)
		return;

	for (const auto& pChild : m_pOwner->GetChildren())
	{
		if (pChild)
		{
			pChild->GetTransform().MarkWorldDirty();
		}
	}
}

/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*-------------------------------------------------------------------------------------------------
| --- GetParentTransform: Returns the Transform of the owner's parent, or nullptr at the root --- |
-------------------------------------------------------------------------------------------------*/
const CaledonEngine::Transform* CaledonEngine::Transform::GetParentTransform() const
{
	const GameObject* pParent = m_pOwner ? m_pOwner->GetParent() : nullptr;
	return pParent ? &pParent->GetTransform() : nullptr;
}

/*------------------------------------------------------------------------------------------------------
| --- UpdateWorld: Recalculates the cached world pose from the parent's world pose                 --- |
| ---              matrix   = parentMatrix * R(localRotation) * S(localScale)   (exact, may shear) --- |
| ---              position = parentPosition + parentMatrix * localPosition       (exact)          --- |
| ---              rotation = parentRotation +/- localRotation (minus when the parent mirrors)     --- |
| ---              scale    = parentScale * localScale (per axis; lossy, see the class banner)     --- |
------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Transform::UpdateWorld() const
{
	if (!m_isWorldDirty)
		return;

	// Local 2x2 matrix: R(rotation) * S(scale), stored [a c; b d]
	const float radians = m_localRotation * CaledonEngine::Math::Deg2Rad;
	const float cosine = std::cos(radians);
	const float sine = std::sin(radians);
	const float local[4] = { cosine * m_localScale.x, sine * m_localScale.x, -sine * m_localScale.y, cosine * m_localScale.y };

	const Transform* pParent = GetParentTransform();
	if (!pParent)
	{
		m_worldPosition = m_localPosition;
		m_worldRotation = m_localRotation;
		m_worldScale = m_localScale;
		for (int i = 0; i < 4; ++i)
		{
			m_worldLinear[i] = local[i];
		}
	}
	else
	{
		pParent->UpdateWorld();
		const float* p = pParent->m_worldLinear;
		const Vector2f& parentScale = pParent->m_worldScale;
		const float parentRotation = pParent->m_worldRotation;

		m_worldLinear[0] = p[0] * local[0] + p[2] * local[1];
		m_worldLinear[1] = p[1] * local[0] + p[3] * local[1];
		m_worldLinear[2] = p[0] * local[2] + p[2] * local[3];
		m_worldLinear[3] = p[1] * local[2] + p[3] * local[3];

		m_worldPosition = pParent->m_worldPosition + Vector2f{ p[0] * m_localPosition.x + p[2] * m_localPosition.y, p[1] * m_localPosition.x + p[3] * m_localPosition.y };
		m_worldRotation = IsMirrored(parentScale) ? parentRotation - m_localRotation : parentRotation + m_localRotation;
		m_worldScale = Vector2f{ parentScale.x * m_localScale.x, parentScale.y * m_localScale.y };
	}

	m_isWorldDirty = false;
}
