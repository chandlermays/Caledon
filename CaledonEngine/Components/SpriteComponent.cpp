/*------------------------------
| File: SpriteComponent.cpp
| Author: Chandler Mays
------------------------------*/
#include "SpriteComponent.h"

#include "Systems/Engine/EngineContext.h"
#include "Systems/Engine/LoggingSubsystem.h"
#include "Systems/Rendering/Renderer.h"
#include "Systems/Rendering/RenderingSubsystem.h"
#include "Systems/Rendering/Shapes/Shape.h"
#include "Systems/Rendering/Resources/Sprite.h"
#include "Systems/Scene/GameObject.h"
#include "Systems/Scene/Transform.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------
| --- Constructor: Constructs the SpriteComponent with default values --- |
-------------------------------------------------------------------------*/
CaledonEngine::SpriteComponent::SpriteComponent()
	: Component()
	, m_pRenderer{ nullptr }
	, m_pSprite{ nullptr }
	, m_color{ 255, 255, 255, 255 }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::SpriteComponent::~SpriteComponent() = default;

/*----------------------------------------------------------
| --- Initialize: Prepares the SpriteComponent for use --- |
----------------------------------------------------------*/
bool CaledonEngine::SpriteComponent::Initialize(EngineContext& context)
{
	// Cache the renderer. The scene that owns this component must shut down before the RenderingSubsystem does.
	RenderingSubsystem* pRendering = context.Get<RenderingSubsystem>();
	m_pRenderer = pRendering ? pRendering->GetRenderer() : nullptr;

	if (!m_pRenderer)
	{
		CE_LOG(context, LogLevel::Error, "SpriteComponent", "No renderer is available. Register the RenderingSubsystem before the scene that owns this component");
		return false;
	}

	return true;
}

/*------------------------------------------------
| --- Render: Draws the sprite to the screen --- |
------------------------------------------------*/
void CaledonEngine::SpriteComponent::Render()
{
	// Add error logging if this Render method breaks early (with an explanation).

	if (!m_isActive || !m_pOwner || !m_pRenderer || !m_pSprite)
		return;

	const Transform& transform = m_pOwner->GetTransform();

	const float x = transform.GetPosition().x;
	const float y = transform.GetPosition().y;

	const float sx = transform.GetScale().x;
	const float sy = transform.GetScale().y;

	const float scaleX = (sx != 0.0f ? sx : 1.0f);
	const float scaleY = (sy != 0.0f ? sy : 1.0f);

	const Vector2f& pivot = m_pSprite->GetPivot();

	float pixelWidth = m_pSprite->GetPixelWidth() * scaleX;
	float pixelHeight = m_pSprite->GetPixelHeight() * scaleY;

	float offsetX = -pivot.x * pixelWidth;
	float offsetY = -pivot.y * pixelHeight;

	RectFloat destRect
	{
		x + offsetX,
		y + offsetY,
		pixelWidth,
		pixelHeight
	};

	if (m_pSprite->GetType() == SpriteType::kTexture)
	{
		Texture* pTexture = m_pSprite->GetTexture();
		if (pTexture)
		{
			RectInt srcRect = m_pSprite->GetTextureRect();

			if (m_color != Color::White())
			{
				m_pRenderer->SetTextureColorMod(pTexture, m_color.r, m_color.g, m_color.b);
				m_pRenderer->SetTextureAlphaMod(pTexture, m_color.a);
			}

			m_pRenderer->RenderTexture(pTexture, &srcRect, &destRect);

			if (m_color != Color::White())
			{
				m_pRenderer->SetTextureColorMod(pTexture, 255, 255, 255);
				m_pRenderer->SetTextureAlphaMod(pTexture, 255);
			}
		}
	}
	else if (m_pSprite->GetType() == SpriteType::kPrimitive)
	{
		Shape* pShape = m_pSprite->GetShape();
		if (pShape)
		{
			Color renderColor = pShape->GetColor();
			if (m_color != Color::White())
			{
				renderColor.r = static_cast<uint8_t>((renderColor.r * m_color.r) / 255);
				renderColor.g = static_cast<uint8_t>((renderColor.g * m_color.g) / 255);
				renderColor.b = static_cast<uint8_t>((renderColor.b * m_color.b) / 255);
				renderColor.a = static_cast<uint8_t>((renderColor.a * m_color.a) / 255);
			}
			pShape->Render(m_pRenderer, destRect, renderColor);
		}
	}
}

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