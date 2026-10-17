/*------------------------------
| File: CircleCollider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "CircleCollider2D.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------------
| --- Constructor: Constructs this CircleCollider2D with default values --- |
---------------------------------------------------------------------------*/
CaledonEngine::CircleCollider2D::CircleCollider2D()
{}

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::CircleCollider2D::GetTypeName() const
{
    static const std::string typeName = "CircleCollider2D";
    return typeName;
}