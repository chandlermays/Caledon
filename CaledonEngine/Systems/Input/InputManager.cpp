/*------------------------------
| File: InputManager.cpp
| Author: Chandler Mays
------------------------------*/
#include "InputManager.h"

#include "API/SDL/SDLInput.h"
#include "../Engine/EngineContext.h"
#include "../Engine/LoggingSubsystem.h"

#include <format>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------
| --- Constructor: Constructs the InputManager with default values --- |
----------------------------------------------------------------------*/
CaledonEngine::InputSubsystem::InputSubsystem()
	: m_pInputAPI{ nullptr }
	, m_pInputActions{ nullptr }
	, m_pContext{ nullptr }
{
}

/*--------------------------------------------
| --- Name: Returns the Subsystem's name --- |
--------------------------------------------*/
std::string_view CaledonEngine::InputSubsystem::Name() const noexcept
{
	return kName;
}

/*-------------------------------------------------------
| --- Initialize: Starts the platform input backend --- |
-------------------------------------------------------*/
bool CaledonEngine::InputSubsystem::Initialize(EngineContext& context)
{
	m_pInputAPI = std::make_unique<SDLInput>();
	if (!m_pInputAPI->Initialize())
	{
		CE_LOG(context, LogLevel::Error, "Input", std::format("Input backend failed to start: {}", m_pInputAPI->GetLastError()));
		return false;
	}

	m_pContext = &context;

	return true;
}

/*---------------------------------------------------------
| --- Shutdown: Shuts down the platform input backend --- |
---------------------------------------------------------*/
void CaledonEngine::InputSubsystem::Shutdown() noexcept
{
	if (m_pInputAPI)
	{
		m_pInputAPI->Shutdown();
		m_pInputAPI.reset();
	}

	m_pContext = nullptr;
}

/*-------------------------------------------------------------------------------
| --- Update: Processes events and requests a quit when the OS asks for one --- |
-------------------------------------------------------------------------------*/
void CaledonEngine::InputSubsystem::Update([[maybe_unused]] float deltaTime)
{
	if (ProcessEvents() && m_pContext)
	{
		CE_LOG(*m_pContext, LogLevel::Log, "Input", "Quit event received (window closed or OS quit signal): requesting engine shutdown");
		m_pContext->RequestQuit();
	}
}

/*-------------------------------------------------------------------------------------------
| --- ProcessEvents: Processes events and actions; returns true if a quit was requested --- |
-------------------------------------------------------------------------------------------*/
bool CaledonEngine::InputSubsystem::ProcessEvents()
{
	if (m_pInputAPI)
	{
		bool quitRequested = m_pInputAPI->ProcessEvents();

		if (quitRequested)
			return true;

		ProcessInputActions();
	}

	return false;
}

/*-----------------------------------------------------------
| --- SetInputActions: Sets the InputActions to process --- |
-----------------------------------------------------------*/
void CaledonEngine::InputSubsystem::SetInputActions(InputActions* pInputActions)
{
	m_pInputActions = pInputActions;
}

/*---------------------------------------------------------
| --- GetInputAPI: Returns the platform input backend --- |
---------------------------------------------------------*/
CaledonEngine::Input* CaledonEngine::InputSubsystem::GetInputAPI() const
{
	return m_pInputAPI.get();
}

/*----------------------------------------------------------
| --- GetInputActions: Returns the active InputActions --- |
----------------------------------------------------------*/
CaledonEngine::InputActions* CaledonEngine::InputSubsystem::GetInputActions() const
{
	return m_pInputActions;
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*-------------------------------------------------------------------------------------
| --- ProcessInputActions: Processes every action in every enabled InputActionMap --- |
-------------------------------------------------------------------------------------*/
void CaledonEngine::InputSubsystem::ProcessInputActions()
{
	if (!m_pInputActions || !m_pInputActions->IsEnabled())
		return;

	const auto& actionMaps = m_pInputActions->GetInputActionMaps();

	for (const auto& [mapName, pActionMap] : actionMaps)
	{
		if (!pActionMap->IsEnabled())
			continue;

		const auto& actions = pActionMap->GetInputActions();
		for (const auto& [actionName, pAction] : actions)
		{
			ProcessAction(pAction.get());
		}
	}
}

/*--------------------------------------------------------------------------------------
| --- ProcessAction: Reads an action's bindings and passes its value to the action --- |
--------------------------------------------------------------------------------------*/
void CaledonEngine::InputSubsystem::ProcessAction(InputAction* pAction)
{
	if (!pAction || !pAction->IsEnabled())
		return;

	float value = 0.0f;

	if (pAction->GetActionType() == ActionType::kValue)
	{
		value = CalculateAxisValue(pAction);
	}
	else
	{
		const auto& bindings = pAction->GetInputBindings();
		for (const auto& binding : bindings)
		{
			bool isHeld = binding.m_isMouseButton
				? m_pInputAPI->IsMouseButtonHeld(binding.m_mouseCode)
				: m_pInputAPI->IsKeyHeld(binding.m_keyCode);

			if (isHeld)
			{
				value = 1.0f;
				break;
			}
		}
	}

	pAction->Process(value);
}

/*-----------------------------------------------------------------------------------
| --- CalculateAxisValue: Combines an action's bindings into a value in [-1, 1] --- |
-----------------------------------------------------------------------------------*/
float CaledonEngine::InputSubsystem::CalculateAxisValue(InputAction* pAction)
{
	if (!pAction || !m_pInputAPI)
		return 0.0f;

	float value = 0.0f;

	const auto& compositeBindings = pAction->GetCompositeBindings();

	if (!compositeBindings.empty())
	{
		const CompositeBinding& composite = compositeBindings[0];

		// Check positive bindings
		for (const auto& binding : composite.m_positiveBindings)
		{
			if (binding.m_isMouseButton)
			{
				if (m_pInputAPI->IsMouseButtonHeld(binding.m_mouseCode))
				{
					value += 1.0f;
					break;
				}
			}
			else
			{
				if (m_pInputAPI->IsKeyHeld(binding.m_keyCode))
				{
					value += 1.0f;
					break;
				}
			}
		}

		// Check negative bindings
		for (const auto& binding : composite.m_negativeBindings)
		{
			if (binding.m_isMouseButton)
			{
				if (m_pInputAPI->IsMouseButtonHeld(binding.m_mouseCode))
				{
					value -= 1.0f;
					break;
				}
			}
			else
			{
				if (m_pInputAPI->IsKeyHeld(binding.m_keyCode))
				{
					value -= 1.0f;
					break;
				}
			}
		}
	}
	else
	{
		// Otherwise, check regular bindings
		const auto& bindings = pAction->GetInputBindings();
		for (const auto& binding : bindings)
		{
			if (binding.m_isMouseButton)
			{
				if (m_pInputAPI->IsMouseButtonHeld(binding.m_mouseCode))
				{
					value += 1.0f;
					break;
				}
			}
			else
			{
				if (m_pInputAPI->IsKeyHeld(binding.m_keyCode))
				{
					value += 1.0f;
					break;
				}
			}
		}
	}

	// Clamp the value to [-1, 1] range
	if (value > 1.0f) value = 1.0f;
	if (value < -1.0f) value = -1.0f;

	return value;
}