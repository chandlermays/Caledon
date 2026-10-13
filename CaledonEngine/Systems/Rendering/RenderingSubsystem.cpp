/*------------------------------
| File: RenderingSubsystem.cpp
| Author: Chandler Mays
------------------------------*/
#include "RenderingSubsystem.h"

#include "Renderer.h"
#include "API/SDL/SDLRenderer.h"
#include "Systems/Engine/EngineContext.h"
#include "Systems/Engine/LoggingSubsystem.h"
#include "Systems/Rendering/Resources/Sprite.h"
#include "Systems/Rendering/Resources/Texture.h"
#include "Systems/Rendering/Shapes/Shape.h"
#include "Systems/Scene/GameObject.h"
#include "Systems/Scene/Scene.h"
#include "Systems/Scene/SceneSubsystem.h"
#include "Systems/Scene/Transform.h"
#include "Systems/Window/WindowSubsystem.h"
#include "Components/SpriteComponent.h"

#include <algorithm>
#include <format>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------------
| --- Constructor: Constructs the RenderingSubsystem with default values --- |
----------------------------------------------------------------------------*/
CaledonEngine::RenderingSubsystem::RenderingSubsystem()
	: m_pRendererAPI{ nullptr }
	, m_pContext{ nullptr }
	, m_pScenes{ nullptr }
	, m_pRenderTarget{ nullptr }
{ }

/*-----------------------------------------------------------------------------
| --- Destructor: Defined here so Renderer can stay a forward declaration --- |
-----------------------------------------------------------------------------*/
CaledonEngine::RenderingSubsystem::~RenderingSubsystem() = default;

/*---------------------------------------------------------------------------------------
| --- Name:
---------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::RenderingSubsystem::Name() const noexcept
{
	return kName;
}

/*---------------------------------------------------------------------------------------
| --- Initialize:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::RenderingSubsystem::Initialize(EngineContext& context)
{
	WindowSubsystem* pWindowSubsystem = context.Get<WindowSubsystem>();
	if (!pWindowSubsystem || !pWindowSubsystem->GetWindow())
	{
		CE_LOG(context, LogLevel::Error, "Rendering", "No window is open. Register the WindowSubsystem before the RenderingSubsystem");
		return false;
	}

	// Only keep the renderer once it has fully started, so a failure leaves nothing behind.
	std::unique_ptr<Renderer> pRendererAPI = std::make_unique<SDLRenderer>();
	if (!pRendererAPI->Initialize(pWindowSubsystem->GetWindow()))
	{
		CE_LOG(context, LogLevel::Error, "Rendering", std::format("Renderer failed to start: {}", pRendererAPI->GetLastError()));
		return false;
	}

	m_pRendererAPI = std::move(pRendererAPI);
	m_pContext = &context;

	CE_LOG(context, LogLevel::Display, "Rendering", "Renderer started (SDL)");
	return true;
}

/*---------------------------------------------------------------------------------------
| --- Shutdown:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::Shutdown() noexcept
{
	m_pContext = nullptr;
	m_pScenes = nullptr;
	m_pRenderTarget = nullptr;
	m_drawList.clear();

	if (!m_pRendererAPI)
		return;

	m_pRendererAPI->Shutdown();
	m_pRendererAPI.reset();
}

/*---------------------------------------------------------------------------------------
| --- BeginFrame:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::BeginFrame()
{
	if (m_pRendererAPI)
	{
		m_pRendererAPI->BeginFrame();
	}
}

/*---------------------------------------------------------------------------------------
| --- Render:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::Render()
{
	if (!m_pRendererAPI)
		return;

	if (!m_pScenes && m_pContext)
		m_pScenes = m_pContext->Get<SceneSubsystem>();

	const Scene* pScene = m_pScenes ? m_pScenes->GetCurrentScene() : nullptr;
	if (!pScene || !pScene->IsActive())
		return;

	// Cache: one flat list of everything drawable this frame, then sort by layer (stable, so equal layers keep scene order).
	m_drawList.clear();
	for (const auto& pObject : pScene->GetGameObjects())
	{
		if (pObject)
			CacheSprites(*pObject);
	}

	std::stable_sort(m_drawList.begin(), m_drawList.end(),
		[](const SpriteComponent* a, const SpriteComponent* b) { return a->GetLayer() < b->GetLayer(); });		// The use of layer is loosely implemented; more of a placeholder than a concrete feature

	// Draw: a single pass over the sorted list.
	if (m_pRenderTarget)
	{
		m_pRendererAPI->SetRenderTarget(m_pRenderTarget);
		m_pRendererAPI->Clear();
	}

	for (const SpriteComponent* pSprite : m_drawList)
		RenderSprite(*pSprite);

	if (m_pRenderTarget)
		m_pRendererAPI->SetRenderTarget(nullptr);
}

/*---------------------------------------------------------------------------------------
| --- EndFrame:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::EndFrame()
{
	if (m_pRendererAPI)
	{
		m_pRendererAPI->EndFrame();
	}
}

/*---------------------------------------------------------------------------------------
| --- GetRenderTarget:
---------------------------------------------------------------------------------------*/
CaledonEngine::Texture* CaledonEngine::RenderingSubsystem::GetRenderTarget() const noexcept
{
	return m_pRenderTarget;
}

/*---------------------------------------------------------------------------------------
| --- SetRenderTarget:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::SetRenderTarget(Texture* pTarget)
{
	m_pRenderTarget = pTarget;
}

/*---------------------------------------------------------------------------------------
| --- GetRenderer:
---------------------------------------------------------------------------------------*/
CaledonEngine::Renderer* CaledonEngine::RenderingSubsystem::GetRenderer() const noexcept
{
	return m_pRendererAPI.get();
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*---------------------------------------------------------------------------------------
| --- CacheSprites:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::CacheSprites(const GameObject& gameObject)
{
	if (!gameObject.IsActive())
		return;

	for (const auto& pComponent : gameObject.GetAllComponents())
	{
		const SpriteComponent* pSprite = dynamic_cast<const SpriteComponent*>(pComponent.get());
		if (pSprite && pSprite->IsActive() && pSprite->GetSprite())
		{
			m_drawList.push_back(pSprite);
		}
	}

	for (const auto& pChild : gameObject.GetChildren())
	{
		if (pChild)
			CacheSprites(*pChild);
	}
}

/*---------------------------------------------------------------------------------------
| --- RenderSprite:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::RenderSprite(const SpriteComponent& sprite)
{
	const GameObject* pOwner = sprite.GetOwner();
	Sprite* pSprite = sprite.GetSprite();
	if (!pOwner || !pSprite)
		return;

	const Transform& transform = pOwner->GetTransform();

	const float x = transform.GetPosition().x;
	const float y = transform.GetPosition().y;

	const float scaleX = (transform.GetScale().x != 0.0f ? transform.GetScale().x : 1.0f);
	const float scaleY = (transform.GetScale().y != 0.0f ? transform.GetScale().y : 1.0f);

	const Vector2f& pivot = pSprite->GetPivot();

	const float pixelWidth = pSprite->GetPixelWidth() * scaleX;
	const float pixelHeight = pSprite->GetPixelHeight() * scaleY;

	RectFloat destRect
	{
		x - pivot.x * pixelWidth,
		y - pivot.y * pixelHeight,
		pixelWidth,
		pixelHeight
	};

	const Color& tint = sprite.GetColor();
	const bool isTinted = (tint != Color::White());

	if (pSprite->GetType() == SpriteType::kTexture)
	{
		Texture* pTexture = pSprite->GetTexture();
		if (!pTexture)
			return;

		RectInt srcRect = pSprite->GetTextureRect();

		if (isTinted)
		{
			m_pRendererAPI->SetTextureColorMod(pTexture, tint.r, tint.g, tint.b);
			m_pRendererAPI->SetTextureAlphaMod(pTexture, tint.a);
		}

		m_pRendererAPI->RenderTexture(pTexture, &srcRect, &destRect);

		if (isTinted)
		{
			m_pRendererAPI->SetTextureColorMod(pTexture, 255, 255, 255);
			m_pRendererAPI->SetTextureAlphaMod(pTexture, 255);
		}
	}
	else if (pSprite->GetType() == SpriteType::kPrimitive)
	{
		Shape* pShape = pSprite->GetShape();
		if (!pShape)
			return;

		Color renderColor = pShape->GetColor();
		if (isTinted)
		{
			renderColor.r = static_cast<uint8_t>((renderColor.r * tint.r) / 255);
			renderColor.g = static_cast<uint8_t>((renderColor.g * tint.g) / 255);
			renderColor.b = static_cast<uint8_t>((renderColor.b * tint.b) / 255);
			renderColor.a = static_cast<uint8_t>((renderColor.a * tint.a) / 255);
		}

		pShape->Render(m_pRendererAPI.get(), destRect, renderColor);
	}
}