/*------------------------------
| File: Application.cpp
| Author: Chandler Mays
------------------------------*/
#include "Application.h"

#include "CaledonEngine/Systems/Engine/EngineManager.h"
#include "CaledonEngine/Systems/Engine/LoggingSubsystem.h"
#include "CaledonEngine/Systems/Window/WindowSubsystem.H"
#include "CaledonEngine/Systems/Input/InputManager.h"
#include "CaledonEngine/Systems/Module/ComponentRegistry.h"
#include "CaledonEngine/Systems/Module/ModuleSubsystem.h"
#include "CaledonEngine/Systems/Rendering/RenderingSubsystem.h"
#include "CaledonEngine/Systems/Scene/SceneSubsystem.h"

#include "Sandbox.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------
| --- Constructor: Constructs the Application with default values --- |
---------------------------------------------------------------------*/
Game::Application::Application()
	: m_pEngineManager{ nullptr }
{}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
Game::Application::~Application()
{
	Shutdown();
}

/*-------------------------------------------------------------------------------
| --- Initialize:
-------------------------------------------------------------------------------*/
bool Game::Application::Initialize()
{
	m_pEngineManager = std::make_unique<CaledonEngine::EngineManager>();

	// Registration order IS initialization order (shutdown runs in reverse).
	if (!m_pEngineManager->RegisterSubsystem(std::make_unique<CaledonEngine::LoggingSubsystem>()))						return false;
	if (!m_pEngineManager->RegisterSubsystem(std::make_unique<CaledonEngine::WindowSubsystem>()))						return false;
	if (!m_pEngineManager->RegisterSubsystem(std::make_unique<CaledonEngine::InputManager>()))							return false;
	if (!m_pEngineManager->RegisterSubsystem(std::make_unique<CaledonEngine::RenderingSubsystem>()))					return false;
	if (!m_pEngineManager->RegisterSubsystem(std::make_unique<CaledonEngine::ComponentRegistry>()))						return false;
	if (!m_pEngineManager->RegisterSubsystem(std::make_unique<CaledonEngine::ModuleSubsystem>("PacManModule.dll")))		return false;
	if (!m_pEngineManager->RegisterSubsystem(std::make_unique<CaledonEngine::SceneSubsystem>()))						return false;

	if (!m_pEngineManager->RegisterSubsystem(std::make_unique<Sandbox>()))												return false;

	if (!m_pEngineManager->Initialize())
	{
		m_pEngineManager.reset();
		return false;
	}

	return true;
}

/*-------------------------------------------------------------------------------
| --- Run:
-------------------------------------------------------------------------------*/
void Game::Application::Run()
{
	if (m_pEngineManager)
		m_pEngineManager->Run();
}

/*-------------------------------------------------------------------------------
| --- Shutdown:
-------------------------------------------------------------------------------*/
void Game::Application::Shutdown()
{
	if (!m_pEngineManager)
		return;

	m_pEngineManager->Shutdown();
	m_pEngineManager.reset();
}