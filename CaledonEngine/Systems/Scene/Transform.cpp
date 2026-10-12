/*------------------------------
| File: Transform.cpp
| Author: Chandler Mays
------------------------------*/
#include "Transform.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------
| --- Constructor: Constructs the Transform with default values --- |
-------------------------------------------------------------------*/
CaledonEngine::Transform::Transform()
	: Component()
	, m_position{ 0.0f, 0.0f }
	, m_rotation{ 0.0f }
	, m_scale{ 1.0f, 1.0f }
{
}

/*------------------------------------------------------------------------------------------
| --- Parameterized Constructor: Sets initial values for position, rotation, and scale --- |
------------------------------------------------------------------------------------------*/
CaledonEngine::Transform::Transform(const Vector2f& position, float rotation, const Vector2f& scale)
	: Component()
	, m_position{ position }
	, m_rotation{ rotation }
	, m_scale{ scale }
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

/*---------------------------------------------------------
| --- GetPosition: Returns the position of the object --- |
---------------------------------------------------------*/
const CaledonEngine::Vector2f& CaledonEngine::Transform::GetPosition() const
{
	return m_position;
}

/*------------------------------------------------------
| --- SetPosition: Sets the position of the object --- |
------------------------------------------------------*/
void CaledonEngine::Transform::SetPosition(const Vector2f& position)
{
	m_position = position;
}

/*---------------------------------------------------------
| --- GetRotation: Returns the rotation of the object --- |
---------------------------------------------------------*/
float CaledonEngine::Transform::GetRotation() const
{
	return m_rotation;
}

/*------------------------------------------------------
| --- SetRotation: Sets the rotation of the object --- |
------------------------------------------------------*/
void CaledonEngine::Transform::SetRotation(float rotation)
{
	m_rotation = rotation;
}

/*---------------------------------------------------
| --- GetScale: Returns the scale of the object --- |
---------------------------------------------------*/
const CaledonEngine::Vector2f& CaledonEngine::Transform::GetScale() const
{
	return m_scale;
}

/*------------------------------------------------
| --- SetScale: Sets the scale of the object --- |
------------------------------------------------*/
void CaledonEngine::Transform::SetScale(const Vector2f& scale)
{
	m_scale = scale;
}