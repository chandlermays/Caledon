/*------------------------------
| File: Collider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "Collider2D.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------
| --- Constructor: Constructs this Collider2D with default values --- |
---------------------------------------------------------------------*/
CaledonEngine::Collider2D::Collider2D()
{}

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::Collider2D::GetTypeName() const
{
    static const std::string typeName = "Collider2D";
    return typeName;
}

/*------------------------------------------------------------------------------
| --- CanContact: Determines if both Collider2D can ever come into contact --- |
------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::CanContact(Collider2D collider)
{
    return false;
}

/*-----------------------------------------------------------------------------------------------------------------
| --- ClosestPoint: Returns a point on the perimeter of this Collider2D that is closest to the given position --- |
-----------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Collider2D::ClosestPoint(Vector2f position)
{
    return Vector2f();
}

/*---------------------------------------------------------------------------------------------------
| --- Distance: Calculates the minimum seperation of this Collider2D against another Collider2D --- |
---------------------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::Collider2D::Distance(Collider2D collider)
{
    return ColliderDistance2D();
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: Check whether this Collider2D is touching the Collider2D or not --- |
-------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(Collider2D collider)
{
    return false;
}

/*-----------------------------------------------------------------------------------
| --- OverlapPoint: Check if a Collider2D overlaps a given point in world-space --- |
-----------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::OverlapPoint(Vector2f point)
{
    return false;
}