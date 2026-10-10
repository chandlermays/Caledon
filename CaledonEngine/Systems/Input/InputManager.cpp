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
CaledonEngine::InputManager::InputManager()
	: m_pInputAPI{ nullptr }
	, m_pInputActions{ nullptr }
	, m_pContext{ nullptr }
{}

/*---------------------------------------------------------------------------------------
| --- Name:
---------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::InputManager::Name() const noexcept
{
	return kName;
}

/*---------------------------------------------------------------------------------------
| --- Initiailze:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::InputManager::Initialize(EngineContext& context)
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

/*---------------------------------------------------------------------------------------
| --- Shutdown:
---------------------------------------------------------------------------------------*/
void CaledonEngine::InputManager::Shutdown() noexcept
{
	if (m_pInputAPI)
	{
		m_pInputAPI->Shutdown();
		m_pInputAPI.reset();
	}

	m_pContext = nullptr;
}

/*---------------------------------------------------------------------------------------
| --- Update:
---------------------------------------------------------------------------------------*/
void CaledonEngine::InputManager::Update([[maybe_unused]] float deltaTime)
{
	if (ProcessEvents() && m_pContext)
	{
		CE_LOG(*m_pContext, LogLevel::Log, "Input", "Quit event received (window closed or OS quit signal): requesting engine shutdown");
		m_pContext->RequestQuit();
	}
}

/*---------------------------------------------------------------------------------------
| --- ProcessEvents:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::InputManager::ProcessEvents()
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

/*---------------------------------------------------------------------------------------
| --- SetInputActions:
---------------------------------------------------------------------------------------*/
void CaledonEngine::InputManager::SetInputActions(InputActions* pInputActions)
{
	m_pInputActions = pInputActions;
}

/*---------------------------------------------------------------------------------------
| --- GetInputAPI:
---------------------------------------------------------------------------------------*/
CaledonEngine::Input* CaledonEngine::InputManager::GetInputAPI() const
{
	return m_pInputAPI.get();
}

/*---------------------------------------------------------------------------------------
| --- GetInputActions:
---------------------------------------------------------------------------------------*/
CaledonEngine::InputActions* CaledonEngine::InputManager::GetInputActions() const
{
	return m_pInputActions;
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*---------------------------------------------------------------------------------------
| --- ProcessInputActions:
---------------------------------------------------------------------------------------*/
void CaledonEngine::InputManager::ProcessInputActions()
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

/*---------------------------------------------------------------------------------------
| --- ProcessAction:
---------------------------------------------------------------------------------------*/
void CaledonEngine::InputManager::ProcessAction(InputAction* pAction)
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

/*---------------------------------------------------------------------------------------
| --- CalculateAxisValue:
---------------------------------------------------------------------------------------*/
float CaledonEngine::InputManager::CalculateAxisValue(InputAction* pAction)
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