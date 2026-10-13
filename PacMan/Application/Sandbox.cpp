/*------------------------------
| File: Sandbox.cpp
| Author: Chandler Mays
------------------------------*/
#include "Sandbox.h"

#include "CaledonEngine/Components/SpriteComponent.h"
#include "CaledonEngine/Systems/Engine/EngineContext.h"
#include "CaledonEngine/Systems/Engine/LoggingSubsystem.h"
#include "CaledonEngine/Systems/Rendering/Resources/Sprite.h"
#include "CaledonEngine/Systems/Rendering/Shapes/Circle.h"
#include "CaledonEngine/Systems/Scene/GameObject.h"
#include "CaledonEngine/Systems/Scene/Scene.h"
#include "CaledonEngine/Systems/Scene/SceneSubsystem.h"
#include "CaledonEngine/Systems/Scene/Transform.h"
#include "CaledonEngine/Systems/Window/Window.h"
#include "CaledonEngine/Systems/Window/WindowSubsystem.h"

#include <memory>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------------
| --- Name: 
-------------------------------------------------------------------------------*/
std::string_view Game::Sandbox::Name() const noexcept
{
	return kName;
}

/*-------------------------------------------------------------------------------
| --- Initialize:
-------------------------------------------------------------------------------*/
bool Game::Sandbox::Initialize(CaledonEngine::EngineContext& context)
{
	using namespace CaledonEngine;

	SceneSubsystem* pScenes = context.Get<SceneSubsystem>();
	WindowSubsystem* pWindowSubsystem = context.Get<WindowSubsystem>();
	if (!pScenes || !pWindowSubsystem || !pWindowSubsystem->GetWindow())
	{
		CE_LOG(context, LogLevel::Error, "SceneSandbox", "Needs the Window and Scene subsystems.");
		return false;
	}

	const Window& window = *pWindowSubsystem->GetWindow();

	// Create Player GameObject
	auto pGameObject = std::make_unique<GameObject>();
	pGameObject->SetName("Player");
	pGameObject->GetTransform().SetPosition({ window.GetWidth() / 2.0f, window.GetHeight() / 2.0f });

	// 1. Add Sprite Component
	auto pSpriteComponent = std::make_unique<SpriteComponent>();
	pSpriteComponent->SetSprite(Sprite::CreateFromShape(std::make_unique<Circle>(Color::Yellow(), 50)));
	pGameObject->AddComponent(std::move(pSpriteComponent));

	// Create Scene and add GameObject
	auto pScene = std::make_unique<Scene>();
	pScene->SetName("Sandbox");
	pScene->AddGameObject(std::move(pGameObject));

	if (!pScene->Initialize())
	{
		CE_LOG(context, LogLevel::Error, "SceneSandbox", "The test scene failed to initialize");
		return false;
	}

	pScenes->AddScene(std::move(pScene));

	CE_LOG(context, LogLevel::Display, "SceneSandbox", "Test scene ready with visible object (Milestone 2 complete)");
	return true;
}


/*-------------------------------------------------------------------------------
| --- Shutdown:
-------------------------------------------------------------------------------*/
void Game::Sandbox::Shutdown() noexcept
{
	//...
}