/*------------------------------
| File: Shape.cpp
| Author: Chandler Mays
------------------------------*/
#include "Shape.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------
| --- Constructor: Constructs the Shape with default values --- |
---------------------------------------------------------------*/
CaledonEngine::Shape::Shape(ShapeType type, const Color& color, int width, int height, bool isFilled)
	: m_shapeType{ type }
	, m_color{ color }
	, m_width{ width }
	, m_height{ height }
	, m_isFilled{ isFilled }
	, m_bounds{ 0, 0, width, height }
{ }

/*-----------------------------------------------
| --- SetColor: Sets the color of the shape --- |
-----------------------------------------------*/
void CaledonEngine::Shape::SetColor(const Color& color)
{
	m_color = color;
}

/*-----------------------------------------------
| --- SetColor: Sets the color of the shape --- |
-----------------------------------------------*/
void CaledonEngine::Shape::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	m_color = Color{ r, g, b, a };
}

/*-----------------------------------------------
| --- SetWidth: Sets the width of the shape --- |
-----------------------------------------------*/
void CaledonEngine::Shape::SetWidth(int width)
{
	m_width = width;
}

/*-------------------------------------------------
| --- SetHeight: Sets the height of the shape --- |
-------------------------------------------------*/
void CaledonEngine::Shape::SetHeight(int height)
{
	m_height = height;
}

/*---------------------------------------------
| --- SetSize: Sets the size of the shape --- |
---------------------------------------------*/
void CaledonEngine::Shape::SetSize(int width, int height)
{
	m_width = width;
	m_height = height;
}

/*------------------------------------------------------------------------
| --- SetFilled: Sets whether the shape is filled or just an outline --- |
------------------------------------------------------------------------*/
void CaledonEngine::Shape::SetFilled(bool isFilled)
{
	m_isFilled = isFilled;
}

/*-----------------------------------------------------
| --- GetShapeType: Returns the type of the shape --- |
-----------------------------------------------------*/
CaledonEngine::ShapeType CaledonEngine::Shape::GetShapeType() const
{
	return m_shapeType;
}

/*----------------------------------------------------------------
| --- GetBounds: Returns the bounding rectangle of the shape --- |
----------------------------------------------------------------*/
CaledonEngine::RectInt CaledonEngine::Shape::GetBounds() const
{
	return m_bounds;
}

/*--------------------------------------------------
| --- GetColor: Returns the color of the shape --- |
--------------------------------------------------*/
const CaledonEngine::Color& CaledonEngine::Shape::GetColor() const
{
	return m_color;
}

/*--------------------------------------------------
| --- GetWidth: Returns the width of the shape --- |
--------------------------------------------------*/
int CaledonEngine::Shape::GetWidth() const
{
	return m_width;
}

/*----------------------------------------------------
| --- GetHeight: Returns the height of the shape --- |
----------------------------------------------------*/
int CaledonEngine::Shape::GetHeight() const
{
	return m_height;
}

/*---------------------------------------------------------------------------
| --- GetFilled: Returns whether the shape is filled or just an outline --- |
---------------------------------------------------------------------------*/
bool CaledonEngine::Shape::IsFilled() const
{
	return m_isFilled;
}