/*------------------------------
| File: PacmanInputActions.cpp
| Author: Chandler Mays
------------------------------*/
#include "PacmanInputActions.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------------
| --- Constructor: Constructs the PacmanInputActions with default values --- |
----------------------------------------------------------------------------*/
PacmanModule::PacmanInputActions::PacmanInputActions()
	: CaledonEngine::InputActions("PacmanInputActions")
	, m_pGameplayActionMap{ nullptr }
{}

/*----------------------------------------------------------------------------
| --- Initialize: 
----------------------------------------------------------------------------*/
void PacmanModule::PacmanInputActions::Initialize()
{
	// Early return if the map has already been initialized
	if (m_pGameplayActionMap)
		return;

	ConfigureGameplayMap();
}

/*----------------------------------------------------------------------------
| --- GetGameplayActionMap: 
----------------------------------------------------------------------------*/
CaledonEngine::InputActionMap* PacmanModule::PacmanInputActions::GetGameplayActionMap() const
{
	return m_pGameplayActionMap;
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*----------------------------------------------------------------------------
| --- ConfigureGameplayMap:
----------------------------------------------------------------------------*/
void PacmanModule::PacmanInputActions::ConfigureGameplayMap()
{
	using CaledonEngine::ActionType;
	using CaledonEngine::ControlType;
	using CaledonEngine::KeyCode;

	// If a "Gameplay" map already exists, adopt it instead of adding a duplicate
	if (CaledonEngine::InputActionMap* pExists = GetActionMapByName("Gameplay"))
	{
		m_pGameplayActionMap = pExists;
		return;
	}

	AddActionMap("Gameplay");

	// Ensure the action map was created successfully
	m_pGameplayActionMap = GetActionMapByName("Gameplay");
	if (!m_pGameplayActionMap)
		return;

	// -- Movement Input Actions --

	// Horizontal Movement
	m_pGameplayActionMap->AddAction("MoveHorizontal", ActionType::kValue);
	CaledonEngine::InputAction* pMoveHorizontal = m_pGameplayActionMap->GetActionByName("MoveHorizontal");
	if (pMoveHorizontal)
	{
		pMoveHorizontal->SetControlType(ControlType::kAxis);
		pMoveHorizontal->AddPositiveBinding(KeyCode::kD);
		pMoveHorizontal->AddPositiveBinding(KeyCode::kRight);
		pMoveHorizontal->AddNegativeBinding(KeyCode::kA);
		pMoveHorizontal->AddNegativeBinding(KeyCode::kLeft);
	}

	// Vertical Movement
	m_pGameplayActionMap->AddAction("MoveVertical", ActionType::kValue);
	CaledonEngine::InputAction* pMoveVertical = m_pGameplayActionMap->GetActionByName("MoveVertical");
	if (pMoveVertical)
	{
		pMoveVertical->SetControlType(ControlType::kAxis);
		pMoveVertical->AddPositiveBinding(KeyCode::kW);
		pMoveVertical->AddPositiveBinding(KeyCode::kUp);
		pMoveVertical->AddNegativeBinding(KeyCode::kS);
		pMoveVertical->AddNegativeBinding(KeyCode::kDown);
	}

	m_pGameplayActionMap->Enable();
}