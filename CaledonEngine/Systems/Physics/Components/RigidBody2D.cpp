/*------------------------------
| File: RigidBody2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "RigidBody2D.h"

#include "Systems/Physics/Components/Collider2D.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------
| --- Constructor: Constructs this RigidBody2D with default values --- |
----------------------------------------------------------------------*/
CaledonEngine::RigidBody2D::RigidBody2D()
{}

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::RigidBody2D::GetTypeName() const
{
	static const std::string typeName = "RigidBody2D";
	return typeName;
}

/*-----------------------------------------------------
| --- AddForce: Apply a force to this RigidBody2D --- |
-----------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForce(Vector2f force, ForceMode2D mode)
{}

/*------------------------------------------------------------------------------
| --- AddForceAtPosition: Apply a force at a given position in world-space --- |
------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceAtPosition(Vector2f force, Vector2f position, ForceMode2D mode)
{}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- AddForceX: Adds a force to the X component of the linear velocity only leaving the Y component of the world-space untouched --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceX(float force, ForceMode2D mode)
{}

/*-------------------------------------------------------------------------------------------------------------------------------------
| --- AddForceY: Adds a force to the Y component of the linear velocity only leaving the X component of the world-space untouched --- |
-------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddForceY(float force, ForceMode2D mode)
{}

/*---------------------------------------------------------------------------
| --- AddRelativeForce: Adds a force to the local-space linear velocity --- |
---------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForce(Vector2f relativeForce, ForceMode2D mode)
{}

/*---------------------------------------------------------------------------------------------------------------------------------------------
| --- AddRelativeForceX: Adds a force to the X component of the linear velocity only leaving the Y component of the local-space untouched --- |
---------------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForceX(float force, ForceMode2D mode)
{}

/*---------------------------------------------------------------------------------------------------------------------------------------------
| --- AddRelativeForceY: Adds a force to the Y component of the linear velocity only leaving the X component of the local-space untouched --- |
---------------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddRelativeForceY(float force, ForceMode2D mode)
{}

/*------------------------------------------------------------------------
| --- AddTorque: Apply a torque at this RigidBody2D's center of mass --- |
------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::AddTorque(float torque, ForceMode2D mode)
{}

/*------------------------------------------------------------------------------------------------------------------------------------------------------
| --- ClosestPoint: Returns a point on the perimeter of all enabled Colliders attached to this RigidBody that is closest to the specified position --- |
------------------------------------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::ClosestPoint(Vector2f position)
{
	return Vector2f();
}

/*------------------------------------------------------------------------------------------------------------------------
| --- Distance: Calculates the minimum distance of this collider against all Collider2D attached to this RigidBody2D --- |
------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::RigidBody2D::Distance(Collider2D collider)
{
	return ColliderDistance2D();
}

/*----------------------------------------------------------------------------------------
| --- GetPoint: 
----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetPoint(Vector2f point)
{
	return Vector2f();
}

/*----------------------------------------------------------------------------------------
| --- GetPointVelocity:
----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetPointVelocity(Vector2f point)
{
	return Vector2f();
}

/*----------------------------------------------------------------------------------------
| --- GetRelativePoint:
----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativePoint(Vector2f relativePoint)
{
	return Vector2f();
}

/*----------------------------------------------------------------------------------------
| --- GetRelativePointVelocity:
----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativePointVelocity(Vector2f relativePoint)
{
	return Vector2f();
}

/*----------------------------------------------------------------------------------------
| --- GetRelativeVector:
----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetRelativeVector(Vector2f relativeVector)
{
	return Vector2f();
}

/*----------------------------------------------------------------------------------------
| --- GetVector:
----------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::RigidBody2D::GetVector(Vector2f vector)
{
	return Vector2f();
}

/*-----------------------------------------------
| --- IsAwake: Is this RigidBody2D 'Awake'? --- |
-----------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsAwake()
{
	return false;
}

/*-----------------------------------------------------
| --- IsSleeping: Is this RigidBody2D 'Sleeping'? --- |
-----------------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsSleeping()
{
	return false;
}

/*------------------------------------------------------------------------------------------------------------------------
| --- IsTouching: Checks whether the collider is touching any of the collider(s) attached to this RigidBody2D or not --- |
------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::IsTouching(Collider2D collider)
{
	return false;
}

/*--------------------------------------------------------------------
| --- MovePosition: Moves this RigidBody2D to the given position --- |
--------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MovePosition(Vector2f position)
{}

/*--------------------------------------------------------------------------------------------------
| --- MovePositionAndRotation: Moves this RigidBody2D to the given position and rotation angle --- |
--------------------------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MovePositionAndRotation(Vector2f position, float angle)
{}

/*--------------------------------------------------------------------------
| --- MoveRotation: Moves this RigidBody2D to the given rotation angle --- |
--------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::MoveRotation(float angle)
{}

/*------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Check if any of the RigidBody2D colliders overlap a given point in world-space --- |
------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::RigidBody2D::OverlapPoint(Vector2f point)
{
	return false;
}

/*-------------------------------------------------------------------------------
| --- SetRotation: Sets the rotation of this RigidBody2D to the given angle --- |
-------------------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::SetRotation(float angle)
{}

/*----------------------------------------------
| --- Sleep: Make this RigidBody2D 'sleep' --- |
----------------------------------------------*/
void CaledonEngine::RigidBody2D::Sleep()
{}

/*--------------------------------------------------------------------------------------------------------------------------
| --- Slide: Slide this RigidBody2D using the given velocity integrated over deltaTime using the specified SlideConfig --- |
--------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::SlideResults2D CaledonEngine::RigidBody2D::Slide(Vector2f velocity, float deltaTime, SlideConfig2D slideConfig)
{
	return SlideResults2D();
}

/*-------------------------------------------------------------------
| --- WakeUp: Disables the 'sleeping' state of this RigidBody2D --- |
-------------------------------------------------------------------*/
void CaledonEngine::RigidBody2D::WakeUp()
{}