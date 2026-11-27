/*------------------------------
| File: PacmanModule.cpp
| Author: Chandler Mays
------------------------------*/
#include "CaledonEngine/Systems/Module/ModuleInterface.h"

#include "Components/PacmanController.h"
#include "Components/MovementComponent.h"
#include "Input/PacmanInputActions.h"

#include <memory>

namespace
{
	std::shared_ptr<PacmanModule::PacmanInputActions> g_pInputActions;								// The game's InputActions, shared by every PacmanController

	/*----------------------------------------------------------------------------------
	| --- CreateMovementComponent: Creates a MovementComponent with PacMan's speed --- |
	----------------------------------------------------------------------------------*/
	CaledonEngine::Component* CreateMovementComponent()
	{
		auto* pMovement = new PacmanModule::MovementComponent();
		pMovement->SetSpeed(250.0f);
		return pMovement;
	}

	/*---------------------------------------------------------------------------------------------
	| --- CreatePacmanController: Creates a PacmanController wired to the game's InputActions --- |
	---------------------------------------------------------------------------------------------*/
	CaledonEngine::Component* CreatePacmanController()
	{
		auto* pController = new PacmanModule::PacmanController();
		pController->SetInputActions(g_pInputActions);
		return pController;
	}
}

/*--------------------------------------------------------------------------------------------------
| --- CaledonModule_Register: Registers the game's Components and InputActions with the engine --- |
--------------------------------------------------------------------------------------------------*/
CALEDON_MODULE_API bool CaledonModule_Register(const CaledonEngine::ModuleInterface::Host* pHost)
{
	if (!pHost || pHost->version != CaledonEngine::ModuleInterface::kVersion)
		return false;

	g_pInputActions = std::make_shared<PacmanModule::PacmanInputActions>();
	g_pInputActions->Initialize();

	if (!pHost->RegisterComponent(pHost->user, "MovementComponent", &CreateMovementComponent)
		|| !pHost->RegisterComponent(pHost->user, "PacmanController", &CreatePacmanController))
	{
		g_pInputActions.reset();
		return false;
	}

	pHost->SetInputActions(pHost->user, g_pInputActions.get());
	return true;
}

/*--------------------------------------------------------------------
| --- CaledonModule_Unregister: Releases the game's InputActions --- |
--------------------------------------------------------------------*/
CALEDON_MODULE_API void CaledonModule_Unregister(const CaledonEngine::ModuleInterface::Host*)
{
	g_pInputActions.reset();
}