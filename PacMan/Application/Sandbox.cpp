/*------------------------------
| File: Sandbox.cpp
| Author: Chandler Mays
------------------------------*/
#include "Sandbox.h"

#include "CaledonEngine/Components/SpriteComponent.h"
#include "CaledonEngine/Systems/Engine/EngineContext.h"
#include "CaledonEngine/Systems/Engine/LoggingSubsystem.h"
#include "CaledonEngine/Systems/Rendering/RenderingSubsystem.h"
#include "CaledonEngine/Systems/Rendering/Resources/Sprite.h"
#include "CaledonEngine/Systems/Rendering/Shapes/Square.h"
#include "CaledonEngine/Systems/Scene/GameObject.h"
#include "CaledonEngine/Systems/Scene/Transform.h"
#include "CaledonEngine/Systems/Window/Window.h"
#include "CaledonEngine/Systems/Window/WindowSubsystem.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------
| --- Constructor: Constructs the Sandbox with no scene yet --- |
----------------------------------------------------------------------*/
Game::Sandbox::Sandbox()
	: m_pRendering{ nullptr }
	, m_pGameObject{ nullptr }
{
}

/*--------------------------------------------------------------------
| --- Destructor: Defined here so GameObject can stay a forward decl --- |
--------------------------------------------------------------------*/
Game::Sandbox::~Sandbox() = default;

/*-------------------------------------------------------------------------------
| --- Name: Unique lookup key for this subsystem ---
-------------------------------------------------------------------------------*/
std::string_view Game::Sandbox::Name() const noexcept
{
	return kName;
}

/*-------------------------------------------------------------------------------
| --- Initialize: Builds one GameObject with a colored square sprite, centered in
|                 the window. On failure nothing is left behind. ---
-------------------------------------------------------------------------------*/
bool Game::Sandbox::Initialize(CaledonEngine::EngineContext& context)
{
	using namespace CaledonEngine;

	RenderingSubsystem* pRendering = context.Get<RenderingSubsystem>();
	WindowSubsystem* pWindowSubsystem = context.Get<WindowSubsystem>();
	if (!pRendering || !pWindowSubsystem || !pWindowSubsystem->GetWindow())
	{
		CE_LOG(context, LogLevel::Error, "Sandbox", "Needs the Window and Rendering subsystems. Register it after them");
		return false;
	}

	const Window& window = *pWindowSubsystem->GetWindow();

	auto pSpriteComponent = std::make_unique<SpriteComponent>();
	pSpriteComponent->SetSprite(Sprite::CreateFromShape(std::make_unique<Square>(Color::Orange(), 100)));

	auto pGameObject = std::make_unique<GameObject>();
	pGameObject->SetName("Box");
	pGameObject->GetTransform().SetPosition({ window.GetWidth() / 2.0f, window.GetHeight() / 2.0f });
	pGameObject->AddComponent(std::move(pSpriteComponent));

	if (!pGameObject->Initialize(context))
	{
		CE_LOG(context, LogLevel::Error, "Sandbox", "The test GameObject failed to initialize");
		return false;
	}

	m_pGameObject = std::move(pGameObject);
	m_pRendering = pRendering;
	m_pRendering->AddRenderClient(*this);

	CE_LOG(context, LogLevel::Display, "Sandbox", "Test scene ready (1 GameObject)");
	return true;
}

/*-------------------------------------------------------------------------------
| --- Shutdown: Stops drawing, then destroys the GameObjects ---
-------------------------------------------------------------------------------*/
void Game::Sandbox::Shutdown() noexcept
{
	if (m_pRendering)
	{
		m_pRendering->RemoveRenderClient(*this);
		m_pRendering = nullptr;
	}

	m_pGameObject.reset();
}

/*-------------------------------------------------------------------------------
| --- Update: Updates the GameObjects ---
-------------------------------------------------------------------------------*/
void Game::Sandbox::Update(float deltaTime)
{
	if (m_pGameObject)
		m_pGameObject->Update(deltaTime);
}

/*-------------------------------------------------------------------------------
| --- OnRender: Draws the GameObjects (called between BeginFrame and EndFrame) ---
-------------------------------------------------------------------------------*/
void Game::Sandbox::OnRender(CaledonEngine::Renderer&)
{
	if (m_pGameObject)
		m_pGameObject->Render();
}