/*------------------------------
| File: Sandbox.cpp
| Author: Chandler Mays
------------------------------*/
#include "Sandbox.h"

#include "CaledonEngine/Systems/Engine/EngineContext.h"
#include "CaledonEngine/Systems/Engine/LoggingSubsystem.h"
#include "CaledonEngine/Systems/Input/Input.h"
#include "CaledonEngine/Systems/Input/InputManager.h"
#include "CaledonEngine/Systems/Module/ComponentRegistry.h"
#include "CaledonEngine/Systems/Physics/Components/BoxCollider2D.h"
#include "CaledonEngine/Systems/Physics/Components/CircleCollider2D.h"
#include "CaledonEngine/Systems/Physics/Components/Collider2D.h"
#include "CaledonEngine/Systems/Physics/Components/RigidBody2D.h"
#include "CaledonEngine/Systems/Physics/Physics2DSubsystem.h"
#include "CaledonEngine/Systems/Rendering/Components/SpriteComponent.h"
#include "CaledonEngine/Systems/Rendering/Resources/Sprite.h"
#include "CaledonEngine/Systems/Rendering/Shapes/Circle.h"
#include "CaledonEngine/Systems/Rendering/Shapes/Square.h"
#include "CaledonEngine/Systems/Scene/Component.h"
#include "CaledonEngine/Systems/Scene/GameObject.h"
#include "CaledonEngine/Systems/Scene/Scene.h"
#include "CaledonEngine/Systems/Scene/SceneSubsystem.h"
#include "CaledonEngine/Systems/Scene/Transform.h"
#include "CaledonEngine/Systems/Window/Window.h"
#include "CaledonEngine/Systems/Window/WindowSubsystem.h"

#include <memory>
#include <string>

namespace
{
	using namespace CaledonEngine;

	/*---------------------------------------------------------------------------------------------------------------
	| --- MakePlayer: Creates the yellow circle Player (radius 50) with a CircleCollider2D and a RigidBody2D --- |
	| ---             Its rotation is frozen unless freezeRotation is false, which lets friction spin it so it rolls         --- |
	---------------------------------------------------------------------------------------------------------------*/
	std::unique_ptr<GameObject> MakePlayer(const Vector2f& position, float gravityScale, float bounciness = 0.0f, bool freezeRotation = true)
	{
		auto pSprite = std::make_unique<SpriteComponent>();
		pSprite->SetSprite(Sprite::CreateFromShape(std::make_unique<Circle>(Color::Yellow(), 50)));

		auto pCollider = std::make_unique<CircleCollider2D>();
		pCollider->SetRadius(50.0f);
		pCollider->SetBounciness(bounciness);

		auto pBody = std::make_unique<RigidBody2D>();
		pBody->SetGravityScale(gravityScale);
		pBody->SetFreezeRotation(freezeRotation);

		auto pPlayer = std::make_unique<GameObject>();
		pPlayer->SetName("Player");
		pPlayer->GetTransform().SetPosition(position);
		pPlayer->AddComponent(std::move(pSprite));
		pPlayer->AddComponent(std::move(pCollider));
		pPlayer->AddComponent(std::move(pBody));
		return pPlayer;
	}

	/*-------------------------------------------------------------------------------------------------
	| --- MakeMarker: Creates a small black dot with no collider. A circle looks the same at any   --- |
	| ---              angle, so the dot is placed on the Player's rim to show how far it has turned --- |
	-------------------------------------------------------------------------------------------------*/
	std::unique_ptr<GameObject> MakeMarker()
	{
		auto pSprite = std::make_unique<SpriteComponent>();
		pSprite->SetSprite(Sprite::CreateFromShape(std::make_unique<Circle>(Color::Black(), 8)));
		pSprite->SetLayer(1);		// Drawn after the Player (layer 0), so it sits on top

		auto pMarker = std::make_unique<GameObject>();
		pMarker->SetName("RollMarker");
		pMarker->AddComponent(std::move(pSprite));
		return pMarker;
	}

	/*-----------------------------------------------------------------------------------------------------------------------
	| --- MakeBrick: Creates a 50x50 colored box with a BoxCollider2D and a RigidBody2D, so it falls, stacks and tumbles --- |
	-----------------------------------------------------------------------------------------------------------------------*/
	std::unique_ptr<GameObject> MakeBrick(const Vector2f& position, const Color& color)
	{
		auto pSprite = std::make_unique<SpriteComponent>();
		pSprite->SetSprite(Sprite::CreateFromShape(std::make_unique<Square>(color, 50)));

		auto pCollider = std::make_unique<BoxCollider2D>();
		pCollider->SetSize({ 50.0f, 50.0f });

		auto pBrick = std::make_unique<GameObject>();
		pBrick->SetName("Brick");
		pBrick->GetTransform().SetPosition(position);
		pBrick->AddComponent(std::move(pSprite));
		pBrick->AddComponent(std::move(pCollider));
		pBrick->AddComponent(std::make_unique<RigidBody2D>());
		return pBrick;
	}

	/*-------------------------------------------------------------------------------------------------------------------------------
	| --- MakeBlock: Creates a gray block. A 100x100 square scaled to the wanted size, with no RigidBody2D so it is static --- |
	-------------------------------------------------------------------------------------------------------------------------------*/
	std::unique_ptr<GameObject> MakeBlock(const std::string& name, const Vector2f& position, const Vector2f& size, float bounciness = 0.0f, float rotation = 0.0f)
	{
		auto pSprite = std::make_unique<SpriteComponent>();
		pSprite->SetSprite(Sprite::CreateFromShape(std::make_unique<Square>(Color::Gray(), 100)));

		auto pCollider = std::make_unique<BoxCollider2D>();
		pCollider->SetSize({ 100.0f, 100.0f });		// The collider scales with the Transform, so it covers the sprite
		pCollider->SetBounciness(bounciness);

		auto pBlock = std::make_unique<GameObject>();
		pBlock->SetName(name);
		pBlock->GetTransform().SetPosition(position);
		pBlock->GetTransform().SetScale({ size.x / 100.0f, size.y / 100.0f });
		pBlock->GetTransform().SetRotation(rotation);		// In degrees. The sprite and the collider both follow it
		pBlock->AddComponent(std::move(pSprite));
		pBlock->AddComponent(std::move(pCollider));
		return pBlock;
	}
}

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
	Physics2DSubsystem* pPhysics = context.Get<Physics2DSubsystem>();
	InputSubsystem* pInput = context.Get<InputSubsystem>();
	if (!pScenes || !pRegistry || !pPhysics || !pInput || !pWindowSubsystem || !pWindowSubsystem->GetWindow())
	{
		CE_LOG(context, LogLevel::Error, "SceneSandbox", "Needs the Window, Input, Scene, Physics2D and ComponentRegistry subsystems. Register it after them");
		return false;
	}

	m_pContext = &context;

	// The world is measured in pixels (y points down) and the defaults are tuned for meters, so rescale the settings that depend on units
	Physics2DSettings settings = pPhysics->Settings();
	settings.gravity = { 0.0f, 0.0f };								// Each Sandbox sets its own gravity when it loads
	settings.maxTranslationSpeed = 10000.0f;						// Default is 100 units per second, which would cap the player at 100 pixels per second
	settings.maxLinearCorrection = 10.0f;							// Largest overlap fixed in a single step, in pixels
	settings.maxRotationSpeed = 3000.0f;							// Default is 360 degrees per second, which would stop a radius 50 Player from keeping up with its ground speed (rolling needs v / r)
	settings.bounceThreshold = 150.0f;								// Closing speeds below this do not bounce. Default is 1 unit per second, which leaves a bouncing ball chattering on the floor forever
	pPhysics->SetSettings(settings);

	return LoadSandbox(1);
}

/*-------------------------------------------------------------------------------
| --- Shutdown:
-------------------------------------------------------------------------------*/
void Game::Sandbox::Shutdown() noexcept
{
	m_pActiveScene = nullptr;
	m_activeSandbox = 0;
	m_pContext = nullptr;
}

/*-------------------------------------------------------------------------------
| --- Update: Switches Sandbox when the 1, 2, 3, 4 or 5 key is pressed --- |
-------------------------------------------------------------------------------*/
void Game::Sandbox::Update([[maybe_unused]] float deltaTime)
{
	using namespace CaledonEngine;

	if (m_pContext == nullptr)
		return;

	InputSubsystem* pInput = m_pContext->Get<InputSubsystem>();
	const Input* pInputAPI = pInput ? pInput->GetInputAPI() : nullptr;
	if (pInputAPI == nullptr)
		return;

	// Pressing the key of the Sandbox that is already active reloads it, which resets the scene
	if (pInputAPI->IsKeyPressed(KeyCode::k1))		LoadSandbox(1);
	else if (pInputAPI->IsKeyPressed(KeyCode::k2))	LoadSandbox(2);
	else if (pInputAPI->IsKeyPressed(KeyCode::k3))	LoadSandbox(3);
	else if (pInputAPI->IsKeyPressed(KeyCode::k4))  LoadSandbox(4);
	else if (pInputAPI->IsKeyPressed(KeyCode::k5))  LoadSandbox(5);

	// Keep the marker on the rim of the roller, turned by the roller's rotation (it is not a child, so it has to be moved by hand)
	if (m_pRoller != nullptr && m_pRollMarker != nullptr)
	{
		const Transform& roller = m_pRoller->GetTransform();
		constexpr float kMarkerDistance = 35.0f;		// From the center of the Player (radius 50) to the dot
		m_pRollMarker->GetTransform().SetPosition(roller.GetPosition() + Vector2f{ kMarkerDistance, 0.0f }.Rotate(roller.GetRotation()));
	}
}

/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------
| --- LoadSandbox: Destroys the active Sandbox scene and builds the requested one in its place, so only one is ever active --- |
| ---              Every Scene shares the one default PhysicsScene2D, so the old scene is removed (destroying its colliders)  --- |
-------------------------------------------------------------------------------------------------------------------------------*/
bool Game::Sandbox::LoadSandbox(int sandbox)
{
	using namespace CaledonEngine;

	SceneSubsystem* pScenes = m_pContext->Get<SceneSubsystem>();
	ComponentRegistry* pRegistry = m_pContext->Get<ComponentRegistry>();
	Physics2DSubsystem* pPhysics = m_pContext->Get<Physics2DSubsystem>();
	const Window& window = *m_pContext->Get<WindowSubsystem>()->GetWindow();

	const float width = static_cast<float>(window.GetWidth());
	const float height = static_cast<float>(window.GetHeight());

	// Turn off the Sandbox that is active
	if (m_pActiveScene != nullptr)
	{
		pScenes->RemoveScene(m_pActiveScene);
		m_pActiveScene = nullptr;
		m_activeSandbox = 0;
	}

	m_pRoller = nullptr;		// These pointed into the scene that was just removed
	m_pRollMarker = nullptr;

	auto pScene = std::make_unique<Scene>();

	switch (sandbox)
	{
	case 1:
	{
		// Sandbox 1: top-down, no gravity. The Player is moved with the input and collides with two walls
		pPhysics->SetGravity({ 0.0f, 0.0f });
		pScene->SetName("Sandbox1");

		auto pPlayer = MakePlayer({ width / 2.0f, height / 2.0f }, 0.0f);

		// Game-side components come from the module and are created by name; the Game never links against them.
		for (const char* pName : { "MovementComponent", "PacmanController" })
		{
			std::unique_ptr<Component> pComponent = pRegistry->Create(pName);
			if (!pComponent)
			{
				CE_LOG(*m_pContext, LogLevel::Error, "SceneSandbox", std::string{ "The module did not register '" } + pName + "'");
				return false;
			}
			pPlayer->AddComponent(std::move(pComponent));
		}

		pScene->AddGameObject(std::move(pPlayer));
		pScene->AddGameObject(MakeBlock("Wall01", { width * 0.75f, height / 2.0f }, { 40.0f, 300.0f }));
		pScene->AddGameObject(MakeBlock("Wall02", { width * 0.25f, height / 2.0f }, { 40.0f, 300.0f }));
		break;
	}
	case 2:
	{
		// Sandbox 2: gravity (y points down on screen) pulls the Player down onto the Floor. There is no input, so nothing overrides the fall
		pPhysics->SetGravity({ 0.0f, 1500.0f });
		pScene->SetName("Sandbox2");

		pScene->AddGameObject(MakePlayer({ width / 2.0f, 100.0f }, 1.0f));
		pScene->AddGameObject(MakeBlock("Floor", { width / 2.0f, height - 20.0f }, { width, 40.0f }));
		break;
	}
	case 3:
	{
		// Sandbox 3: the same as Sandbox 2, but the Player and the Floor are bouncy (0.8), so the Player loses 20% of its speed on each bounce until it settles
		pPhysics->SetGravity({ 0.0f, 1500.0f });
		pScene->SetName("Sandbox3");

		pScene->AddGameObject(MakePlayer({ width / 2.0f, 100.0f }, 1.0f, 0.8f));
		pScene->AddGameObject(MakeBlock("Floor", { width / 2.0f, height - 20.0f }, { width, 40.0f }, 0.8f));
		break;
	}
	case 4:
	{
		// Sandbox 4: gravity pulls the Player onto a slanted Ramp. It rolls down (no bounce), drops onto the Floor and keeps rolling until it meets a wall
		pPhysics->SetGravity({ 0.0f, 1500.0f });
		pScene->SetName("Sandbox4");

		auto pPlayer = MakePlayer({ width / 2.0f, 100.0f }, 1.0f, 0.0f, false);	// Rotation is free, so friction at the contact spins it
		auto pRamp = MakeBlock("Ramp", { width / 2.0f, height / 2.0f }, { 300.0f, 30.0f }, 0.0f, 25.0f);	// 25 degrees clockwise on screen: the left end is high, the right end is low
		auto pFloor = MakeBlock("Floor", { width / 2.0f, height - 20.0f }, { width, 40.0f });
		auto pWallLeft = MakeBlock("WallLeft", { 20.0f, height - 240.0f }, { 40.0f, 400.0f });
		auto pWallRight = MakeBlock("WallRight", { width - 20.0f, height - 240.0f }, { 40.0f, 400.0f });

		// Enough friction everywhere that the Player grips and rolls instead of slipping (a solid disc needs about tan(25 degrees) / 3 = 0.16 on the Ramp)
		for (GameObject* pObject : { pPlayer.get(), pRamp.get(), pFloor.get(), pWallLeft.get(), pWallRight.get() })
			pObject->GetComponent<Collider2D>()->SetFriction(0.5f);

		auto pMarker = MakeMarker();
		m_pRoller = pPlayer.get();
		m_pRollMarker = pMarker.get();

		pScene->AddGameObject(std::move(pPlayer));
		pScene->AddGameObject(std::move(pMarker));
		pScene->AddGameObject(std::move(pRamp));
		pScene->AddGameObject(std::move(pFloor));
		pScene->AddGameObject(std::move(pWallLeft));
		pScene->AddGameObject(std::move(pWallRight));
		break;
	}
	case 5:
	{
		// Sandbox 5: The Stack. A pyramid of boxes rests on the Floor until a heavy ball rolls in and knocks it down
		pPhysics->SetGravity({ 0.0f, 1500.0f });
		pScene->SetName("Sandbox5");

		constexpr int kRows = 5;
		constexpr float kBrickSize = 50.0f;
		constexpr float kGap = 1.0f;		// A small gap, so the boxes start without overlapping
		const Color rowColors[kRows] = { Color::Orange(), Color::Cyan(), Color::Magenta(), Color::Green(), Color::Purple() };

		const float floorTop = height - 40.0f;
		const float pyramidX = width * 0.65f;
		for (int row = 0; row < kRows; ++row)
		{
			const int bricksInRow = kRows - row;
			const float y = floorTop - kBrickSize / 2.0f - row * (kBrickSize + kGap) - kGap;
			for (int i = 0; i < bricksInRow; ++i)
			{
				const float x = pyramidX + (i - (bricksInRow - 1) / 2.0f) * (kBrickSize + kGap);
				pScene->AddGameObject(MakeBrick({ x, y }, rowColors[row]));
			}
		}

		// The ball is much heavier than a box (mass 30 against 1), so it plows through the pyramid. Its rotation is free, so it rolls
		auto pBall = MakePlayer({ 120.0f, floorTop - 50.0f }, 1.0f, 0.0f, false);
		RigidBody2D* pBallBody = pBall->GetComponent<RigidBody2D>();
		pBallBody->SetMass(30.0f);
		pBallBody->SetLinearVelocity({ 1200.0f, 0.0f });

		auto pMarker = MakeMarker();
		m_pRoller = pBall.get();
		m_pRollMarker = pMarker.get();

		pScene->AddGameObject(std::move(pBall));
		pScene->AddGameObject(std::move(pMarker));
		pScene->AddGameObject(MakeBlock("Floor", { width / 2.0f, height - 20.0f }, { width, 40.0f }));
		pScene->AddGameObject(MakeBlock("WallLeft", { 20.0f, height - 240.0f }, { 40.0f, 400.0f }));
		pScene->AddGameObject(MakeBlock("WallRight", { width - 20.0f, height - 240.0f }, { 40.0f, 400.0f }));
		break;
	}
	default:
		return false;
	}

	Scene* pNewScene = pScene.get();
	if (!pScenes->AddScene(std::move(pScene)))
	{
		CE_LOG(*m_pContext, LogLevel::Error, "SceneSandbox", "The test scene failed to initialize");
		return false;
	}

	pScenes->SetCurrentScene(pNewScene);
	m_pActiveScene = pNewScene;
	m_activeSandbox = sandbox;

	CE_LOG(*m_pContext, LogLevel::Display, "SceneSandbox", "Sandbox " + std::to_string(sandbox) + " is active");
	return true;
}