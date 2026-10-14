/*------------------------------
| File: PacmanController.cpp
| Author: Chandler Mays
------------------------------*/
#include "PacmanController.h"

#include "MovementComponent.h"

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
	, m_horizontalInput{ 0.0f }
	, m_verticalInput{ 0.0f }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
PacmanModule::PacmanController::~PacmanController()
{
	UnsubscribeAll();
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
void PacmanModule::PacmanController::SetInputActions(std::shared_ptr<PacmanInputActions> pInputActions)
{
	// Drop any previous subscriptions first
	UnsubscribeAll();

	m_pInputActions = std::move(pInputActions);
	m_pGameplayActionMap = nullptr;

	if (m_pInputActions)
	{
		m_pInputActions->Initialize();
		m_pGameplayActionMap = m_pInputActions->GetGameplayActionMap();
	}

	ConfigureInputBindings();
}

/*------------------------------------------------------------------------
| --- OnMoveHorizontal:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::OnMoveHorizontal(float value)
{
	m_horizontalInput = value;
	UpdateMovement();
}

/*------------------------------------------------------------------------
| --- OnMoveVertical:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::OnMoveVertical(float value)
{
	m_verticalInput = value;
	UpdateMovement();
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
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

/*------------------------------------------------------------------------
| --- UnsubscribeAll:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::UnsubscribeAll()
{
	for (CaledonEngine::InputAction* pAction : m_subscribedActions)
	{
		if (pAction)
		{
			pAction->Unsubscribe(this);
		}
	}

	m_subscribedActions.clear();
}

/*------------------------------------------------------------------------
| --- UpdateMovement:
------------------------------------------------------------------------*/
void PacmanModule::PacmanController::UpdateMovement()
{
	if (MovementComponent* pMovement = GetComponent<MovementComponent>())
	{
		CaledonEngine::Vector2f direction(m_horizontalInput, -m_verticalInput);
		pMovement->SetDirection(direction);
	}
}