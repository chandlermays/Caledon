/*------------------------------
| File: Sandbox.cpp
| Author: Chandler Mays
------------------------------*/
#include "Sandbox.h"

#include "CaledonEngine/Systems/Engine/EngineContext.h"
#include "CaledonEngine/Systems/Engine/LoggingSubsystem.h"
#include "CaledonEngine/Systems/Module/ComponentRegistry.h"
#include "CaledonEngine/Systems/Rendering/Components/SpriteComponent.h"
#include "CaledonEngine/Systems/Rendering/Resources/Sprite.h"
#include "CaledonEngine/Systems/Rendering/Shapes/Circle.h"
#include "CaledonEngine/Systems/Scene/Component.h"
#include "CaledonEngine/Systems/Scene/GameObject.h"
#include "CaledonEngine/Systems/Scene/Scene.h"
#include "CaledonEngine/Systems/Scene/SceneSubsystem.h"
#include "CaledonEngine/Systems/Scene/Transform.h"
#include "CaledonEngine/Systems/Window/Window.h"
#include "CaledonEngine/Systems/Window/WindowSubsystem.h"

#include <memory>
#include <string>

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
	ComponentRegistry* pRegistry = context.Get<ComponentRegistry>();
	if (!pScenes || !pRegistry || !pWindowSubsystem || !pWindowSubsystem->GetWindow())
	{
		CE_LOG(context, LogLevel::Error, "SceneSandbox", "Needs the Window, Scene and ComponentRegistry subsystems. Register it after them");
		return false;
	}

	const Window& window = *pWindowSubsystem->GetWindow();

	auto pSpriteComponent = std::make_unique<SpriteComponent>();
	pSpriteComponent->SetSprite(Sprite::CreateFromShape(std::make_unique<Circle>(Color::Yellow(), 50)));

	auto pGameObject = std::make_unique<GameObject>();
	pGameObject->SetName("Player");
	pGameObject->GetTransform().SetPosition({ window.GetWidth() / 2.0f, window.GetHeight() / 2.0f });
	pGameObject->AddComponent(std::move(pSpriteComponent));

	// Game-side components come from the module and are created by name; the Game never links against them.
	for (const char* pName : { "MovementComponent", "PacmanController" })
	{
		std::unique_ptr<Component> pComponent = pRegistry->Create(pName);
		if (!pComponent)
		{
			CE_LOG(context, LogLevel::Error, "SceneSandbox", std::string{ "The module did not register '" } + pName + "'");
			return false;
		}
		pGameObject->AddComponent(std::move(pComponent));
	}

	auto pScene = std::make_unique<Scene>();
	pScene->SetName("Sandbox");
	pScene->AddGameObject(std::move(pGameObject));

	if (!pScenes->AddScene(std::move(pScene)))
	{
		CE_LOG(context, LogLevel::Error, "SceneSandbox", "The test scene failed to initialize");
		return false;
	}

	CE_LOG(context, LogLevel::Display, "SceneSandbox", "Test scene ready (Player)");
	return true;
}


/*-------------------------------------------------------------------------------
| --- Shutdown:
-------------------------------------------------------------------------------*/
void Game::Sandbox::Shutdown() noexcept
{
	//...
}