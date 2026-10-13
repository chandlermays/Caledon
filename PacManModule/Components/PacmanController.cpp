/*------------------------------
| File: PacmanController.cpp
| Author: Chandler Mays
------------------------------*/
#include "PacmanController.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*--------------------------------------------------------------------------
| --- Constructor: Constructs the PacmanController with default values --- |
--------------------------------------------------------------------------*/
PacmanModule::PacmanController::PacmanController()
	: CaledonEngine::Component{ }
	, m_pInputActions{ nullptr }
	, m_pGameplayActionMap{ nullptr }
	, m_moveSpeed{ 0 }
	, m_horizontalInput{ 0.0f }
	, m_verticalInput{ 0.0f }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
PacmanModule::PacmanController::~PacmanController()
{
	for (CaledonEngine::InputAction* pAction : m_subscribedActions)
	{
		if (pAction)
		{
			pAction->Unsubscribe(this);
		}
	}
}

/*-------------------------------------------------------------
| --- GetTypeName: Returns the type name of the Component --- |
-------------------------------------------------------------*/
const std::string& PacmanModule::PacmanController::GetTypeName() const
{
	static const std::string typeName = "PacmanController";
	return typeName;
}

/*------------------------------------------------------------------------
| --- SetInputActions:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::SetInputActions(PacmanInputActions* pInputActions)
{
	m_pInputActions = pInputActions;
}

/*------------------------------------------------------------------------
| --- OnMoveHorizontal:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::OnMoveHorizontal(float value)
{
	m_horizontalInput = value;
}

/*------------------------------------------------------------------------
| --- OnMoveVertical:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::OnMoveVertical(float value)
{
	m_verticalInput = value;
}

/*------------------------------------------------------------------------
| --- SetMoveSpeed:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::SetMoveSpeed(float speed)
{
	m_moveSpeed = speed;
}

/*------------------------------------------------------------------------
| --- GetMoveSpeed:
------------------------------------------------------------------------*/
float PacmanModule::PacmanController::GetMoveSpeed() const
{
	return m_moveSpeed;
}

/*------------------------------------------------------------------------
| --- ConfigureInputBindings:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::ConfigureInputBindings()
{
	if (!m_pGameplayActionMap)
		return;

	CaledonEngine::InputAction* pMoveHorizontal = m_pGameplayActionMap->GetActionByName("MoveHorizontal");
	CaledonEngine::InputAction* pMoveVertical = m_pGameplayActionMap->GetActionByName("MoveVertical");

	if (pMoveHorizontal)
	{
		pMoveHorizontal->OnValue(this, &PacmanController::OnMoveHorizontal);
		m_subscribedActions.emplace_back(pMoveHorizontal);
	}

	if (pMoveVertical)
	{
		pMoveVertical->OnValue(this, &PacmanController::OnMoveVertical);
		m_subscribedActions.emplace_back(pMoveVertical);
	}
}