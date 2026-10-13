/*------------------------------
| File: SpriteComponent.cpp
| Author: Chandler Mays
------------------------------*/
#include "SpriteComponent.h"

#include "Systems/Rendering/Resources/Sprite.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------
| --- Constructor: Constructs the SpriteComponent with default values --- |
-------------------------------------------------------------------------*/
CaledonEngine::SpriteComponent::SpriteComponent()
	: Component()
	, m_pSprite{ nullptr }
	, m_color{ 255, 255, 255, 255 }
	, m_layer{ 0 }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::SpriteComponent::~SpriteComponent() = default;

/*---------------------------------------------------
| --- SetSprite: Sets the sprite to be rendered --- |
---------------------------------------------------*/
void CaledonEngine::SpriteComponent::SetSprite(std::unique_ptr<Sprite> pSprite)
{
	m_pSprite = std::move(pSprite);
}

/*-------------------------------------------------
| --- SetColor: Sets the color of the sprite  --- |
-------------------------------------------------*/
void CaledonEngine::SpriteComponent::SetColor(const Color& color)
{
	m_color = color;
}

/*-------------------------------------------------
| --- SetColor: Sets the color of the sprite  --- |
-------------------------------------------------*/
void CaledonEngine::SpriteComponent::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_color.a = a;
}

/*------------------------------------------------------------
| --- SetLayer: Sets the draw order layer of the sprite  --- |
------------------------------------------------------------*/
void CaledonEngine::SpriteComponent::SetLayer(int layer)
{
	m_layer = layer;
}

/*-------------------------------------------------------------
| --- GetTypeName: Returns the type name of the Component --- |
-------------------------------------------------------------*/
const std::string& CaledonEngine::SpriteComponent::GetTypeName() const
{
	static const std::string typeName = "SpriteComponent";
	return typeName;
}

/*----------------------------------------------------
| --- GetSprite: Returns a pointer to the sprite --- |
----------------------------------------------------*/
CaledonEngine::Sprite* CaledonEngine::SpriteComponent::GetSprite() const
{
	return m_pSprite.get();
}

/*---------------------------------------------------
| --- GetColor: Returns the color of the sprite --- |
---------------------------------------------------*/
const CaledonEngine::Color& CaledonEngine::SpriteComponent::GetColor() const
{
	return m_color;
}

/*--------------------------------------------------------------
| --- GetLayer: Returns the draw order layer of the sprite --- |
--------------------------------------------------------------*/
int CaledonEngine::SpriteComponent::GetLayer() const
{
	return m_layer;
}