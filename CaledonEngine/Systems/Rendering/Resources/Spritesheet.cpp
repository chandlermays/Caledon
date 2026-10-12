/*------------------------------
| File: SpriteSheet.cpp
| Author: Chandler Mays
------------------------------*/
#include "SpriteSheet.h"

#include "Systems/Rendering/Resources/Texture.h"
#include "Systems/Rendering/Resources/Sprite.h"

#include <algorithm>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------
| --- Constructor: Constructs the SpriteSheet with default values --- |
---------------------------------------------------------------------*/
CaledonEngine::SpriteSheet::SpriteSheet(std::shared_ptr<Texture> pTexture, int frameWidth, int frameHeight)
	: m_pTexture{ pTexture }
	, m_frameWidth{ frameWidth > 0 ? frameWidth : 1 }
	, m_frameHeight{ frameHeight > 0 ? frameHeight : 1 }
	, m_columns{ 1 }
{
	if (m_pTexture)
	{
		m_columns = std::max(1, m_pTexture->GetWidth() / m_frameWidth);
	}
}

/*--------------------------------------------------------------------------
| --- GetFrameRect: Returns the rectangle of the specified frame index --- |
--------------------------------------------------------------------------*/
CaledonEngine::RectInt CaledonEngine::SpriteSheet::GetFrameRect(int index) const
{
	if (index < 0)
		index = 0;

	int column = index % m_columns;
	int row = index / m_columns;

	return RectInt(column * m_frameWidth, row * m_frameHeight, m_frameWidth, m_frameHeight);
}

/*-----------------------------------------------------------------------
| --- CreateSprite: Creates a sprite from the specified frame index --- |
-----------------------------------------------------------------------*/
std::unique_ptr<CaledonEngine::Sprite> CaledonEngine::SpriteSheet::CreateSprite(int index, const Vector2f& pivot, int pixelsPerUnit) const
{
	if (!m_pTexture)
		return nullptr;

	return Sprite::CreateFromTexture(m_pTexture, GetFrameRect(index), pivot, pixelsPerUnit);
}

/*------------------------------------------------------
| --- GetTexture: Returns a pointer to the texture --- |
------------------------------------------------------*/
CaledonEngine::Texture* CaledonEngine::SpriteSheet::GetTexture() const
{
	return m_pTexture.get();
}

/*------------------------------------------------------------
| --- GetFrameWidth: Returns the width of a single frame --- |
------------------------------------------------------------*/
int CaledonEngine::SpriteSheet::GetFrameWidth() const
{
	return m_frameWidth;
}

/*--------------------------------------------------------------
| --- GetFrameHeight: Returns the height of a single frame --- |
--------------------------------------------------------------*/
int CaledonEngine::SpriteSheet::GetFrameHeight() const
{
	return m_frameHeight;
}

/*------------------------------------------------------------------------------
| --- GetFrameCount: Returns the total number of frames in the spritesheet --- |
------------------------------------------------------------------------------*/
int CaledonEngine::SpriteSheet::GetFrameCount() const
{
	if (!m_pTexture)
		return 0;

	int rows = std::max(1, m_pTexture->GetHeight() / m_frameHeight);
	return m_columns * rows;
}