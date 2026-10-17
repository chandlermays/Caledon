/*------------------------------
| File: BoxCollider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "BoxCollider2D.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------
| --- Constructor: Constructs this BoxCollider2D with default values --- |
------------------------------------------------------------------------*/
CaledonEngine::BoxCollider2D::BoxCollider2D()
{}

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::BoxCollider2D::GetTypeName() const
{
    static const std::string typeName = "BoxCollider2D";
    return typeName;
}