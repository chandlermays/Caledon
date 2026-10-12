/*------------------------------
| File: Sprite.cpp
| Author: Chandler Mays
------------------------------*/
#include "Sprite.h"

#include "Systems/Rendering/Resources/Texture.h"
#include "Systems/Rendering/Shapes/Shape.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------
| --- Constructor: Constructs the Sprite with default values --- |
----------------------------------------------------------------*/
CaledonEngine::Sprite::Sprite()
    : m_type{ SpriteType::kPrimitive }
    , m_pTexture{ nullptr }
    , m_textureRect{ }
    , m_pivot{ 0.5f, 0.5f }
    , m_pShape{ nullptr }
    , m_pixelsPerUnit{ 100 }
    , m_size{ 1.0f, 1.0f }
{
}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::Sprite::~Sprite() = default;

/*------------------------------------------------------------
| --- CreateFromTexture: Creates a sprite from a texture --- |
------------------------------------------------------------*/
std::unique_ptr<CaledonEngine::Sprite> CaledonEngine::Sprite::CreateFromTexture(std::shared_ptr<Texture> pTexture, const RectInt& textureRect, const Vector2f& pivot, int pixelsPerUnit)
{
    std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
    sprite->m_type = SpriteType::kTexture;
    sprite->m_pTexture = pTexture;
    sprite->m_pivot = pivot;
    sprite->m_pixelsPerUnit = pixelsPerUnit;

    if (pTexture)
    {
        if (textureRect.m_width == 0 && textureRect.m_height == 0)
        {
            sprite->m_textureRect = RectInt(0, 0, pTexture->GetWidth(), pTexture->GetHeight());
        }
        else
        {
            sprite->m_textureRect = textureRect;
        }

        sprite->m_size.x = static_cast<float>(sprite->m_textureRect.m_width) / pixelsPerUnit;
        sprite->m_size.y = static_cast<float>(sprite->m_textureRect.m_height) / pixelsPerUnit;
    }

    return sprite;
}

/*--------------------------------------------------------
| --- CreateFromShape: Creates a sprite from a shape --- |
--------------------------------------------------------*/
std::unique_ptr<CaledonEngine::Sprite> CaledonEngine::Sprite::CreateFromShape(std::unique_ptr<Shape> pShape, int pixelsPerUnit)
{
    std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
    sprite->m_type = SpriteType::kPrimitive;
    sprite->m_pShape = std::move(pShape);
    sprite->m_pivot = Vector2f(0.5f, 0.5f);
    sprite->m_pixelsPerUnit = pixelsPerUnit;

    if (sprite->m_pShape)
    {
        sprite->m_size.x = static_cast<float>(sprite->m_pShape->GetWidth()) / pixelsPerUnit;
        sprite->m_size.y = static_cast<float>(sprite->m_pShape->GetHeight()) / pixelsPerUnit;
    }

    return sprite;
}

/*------------------------------------------------------
| --- SetPivot: Sets the pivot point of the sprite --- |
------------------------------------------------------*/
void CaledonEngine::Sprite::SetPivot(const Vector2f& pivot)
{
    m_pivot = pivot;
}

/*--------------------------------------------------------------------------
| --- SetPixelsPerUnit: Sets the number of pixels per unit for scaling --- |
--------------------------------------------------------------------------*/
void CaledonEngine::Sprite::SetPixelsPerUnit(int pixelsPerUnit)
{
    if (pixelsPerUnit > 0)
    {
        m_pixelsPerUnit = pixelsPerUnit;

        if (m_type == SpriteType::kTexture)
        {
            m_size.x = static_cast<float>(m_textureRect.m_width) / pixelsPerUnit;
            m_size.y = static_cast<float>(m_textureRect.m_height) / pixelsPerUnit;
        }
        else if (m_pShape)
        {
            m_size.x = static_cast<float>(m_pShape->GetWidth()) / pixelsPerUnit;
            m_size.y = static_cast<float>(m_pShape->GetHeight()) / pixelsPerUnit;
        }
    }
}

/*-------------------------------------------------
| --- GetType: Returns the type of the sprite --- |
-------------------------------------------------*/
CaledonEngine::SpriteType CaledonEngine::Sprite::GetType() const
{
    return m_type;
}

/*-------------------------------------------------------
| --- GetTexture: Returns the texture of the sprite --- |
-------------------------------------------------------*/
CaledonEngine::Texture* CaledonEngine::Sprite::GetTexture() const
{
    return m_pTexture.get();
}

/*-------------------------------------------------------------------------
| --- GetTextureRect: Returns the rectangle defining the texture area --- |
-------------------------------------------------------------------------*/
const CaledonEngine::RectInt& CaledonEngine::Sprite::GetTextureRect() const
{
    return m_textureRect;
}

/*---------------------------------------------------------------------------
| --- GetTextureRectOffset: Returns the offset of the texture rectangle --- |
---------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Sprite::GetTextureRectOffset() const
{
    Vector2f offset;
    offset.x = -m_pivot.x * static_cast<float>(m_textureRect.m_width);
    offset.y = -m_pivot.y * static_cast<float>(m_textureRect.m_height);
    return offset;
}

/*---------------------------------------------------------
| --- GetPivot: Returns the pivot point of the sprite --- |
---------------------------------------------------------*/
const CaledonEngine::Vector2f& CaledonEngine::Sprite::GetPivot() const
{
    return m_pivot;
}

/*---------------------------------------------------
| --- GetShape: Returns the shape of the sprite --- |
---------------------------------------------------*/
CaledonEngine::Shape* CaledonEngine::Sprite::GetShape() const
{
    return m_pShape.get();
}

/*-----------------------------------------------------------------------------
| --- GetPixelsPerUnit: Returns the number of pixels per unit for scaling --- |
-----------------------------------------------------------------------------*/
int CaledonEngine::Sprite::GetPixelsPerUnit() const
{
    return m_pixelsPerUnit;
}

/*----------------------------------------------------------------
| --- GetSize: Returns the size of the sprite in world units --- |
----------------------------------------------------------------*/
const CaledonEngine::Vector2f& CaledonEngine::Sprite::GetSize() const
{
    return m_size;
}

/*------------------------------------------------------------------
| --- GetPixelWidth: Returns the width of the sprite in pixels --- |
------------------------------------------------------------------*/
int CaledonEngine::Sprite::GetPixelWidth() const
{
    if (m_type == SpriteType::kTexture)
    {
        return m_textureRect.m_width;
    }
    else if (m_pShape)
    {
        return m_pShape->GetWidth();
    }

    return 0;
}

/*--------------------------------------------------------------------
| --- GetPixelHeight: Returns the height of the sprite in pixels --- |
--------------------------------------------------------------------*/
int CaledonEngine::Sprite::GetPixelHeight() const
{
    if (m_type == SpriteType::kTexture)
    {
        return m_textureRect.m_height;
    }
    else if (m_pShape)
    {
        return m_pShape->GetHeight();
    }

    return 0;
}