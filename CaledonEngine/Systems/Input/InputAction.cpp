/*------------------------------
| File: InputAction.cpp
| Author: Chandler Mays
------------------------------*/
#include "InputAction.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------------
| --- Constructor: Constructs a Keyboard Input Binding with default values --- |
------------------------------------------------------------------------------*/
CaledonEngine::InputBinding::InputBinding(KeyCode key)
    : m_keyCode{ key }
    , m_mouseCode{ MouseCode::kNone }
    , m_isMouseButton{ false }
{}

/*----------------------------------------------------------------------------------
| --- Constructor: Constructs a Mouse Button Input Binding with default values --- |
----------------------------------------------------------------------------------*/
CaledonEngine::InputBinding::InputBinding(MouseCode mouse)
    : m_keyCode{ KeyCode::kNone }
    , m_mouseCode{ mouse }
    , m_isMouseButton{ true }
{}

/*--------------------------------------------------------------------
| --- Constructor: Constructs an InputAction with default values --- |
--------------------------------------------------------------------*/
CaledonEngine::InputAction::InputAction(const std::string& name, ActionType type)
    : m_name{ name }
    , m_actionType{ type }
    , m_controlType{ ControlType::kAny }
    , m_initialStateCheck{ false }
    , m_isEnabled{ true }
    , m_wasActive{ false }
{}

/*----------------------------------------------------------------
| --- SetActionType: Sets the action type of an input action --- |
----------------------------------------------------------------*/
void CaledonEngine::InputAction::SetActionType(ActionType type)
{
    m_actionType = type;
}

/*------------------------------------------------------------------
| --- SetControlType: Sets the control type of an input action --- |
------------------------------------------------------------------*/
void CaledonEngine::InputAction::SetControlType(ControlType type)
{
    m_controlType = type;
}

/*-----------------------------------------------------------------
| --- SetInitialStateCheck: Sets the initial state check flag --- |
-----------------------------------------------------------------*/
void CaledonEngine::InputAction::SetInitialStateCheck(bool flag)
{
    m_initialStateCheck = flag;
}

/*----------------------------------------------------------------------
| --- AddBinding: Adds an input binding to a keyboard input action --- |
----------------------------------------------------------------------*/
void CaledonEngine::InputAction::AddBinding(KeyCode key)
{
    m_inputBindings.emplace_back(key);
}

/*-------------------------------------------------------------------
| --- AddBinding: Adds an input binding to a mouse input action --- |
-------------------------------------------------------------------*/
void CaledonEngine::InputAction::AddBinding(MouseCode mouse)
{
    m_inputBindings.emplace_back(mouse);
}

/*------------------------------------------------------------------------------
| --- RemoveBinding: Removes an input binding from a keyboard input action --- |
------------------------------------------------------------------------------*/
void CaledonEngine::InputAction::RemoveBinding(KeyCode key)
{
    m_inputBindings.erase(
        std::remove_if(m_inputBindings.begin(), m_inputBindings.end(),
            [key](const InputBinding& binding)
            {
                return !binding.m_isMouseButton && binding.m_keyCode == key;
            }),
        m_inputBindings.end());
}

/*---------------------------------------------------------------------------
| --- RemoveBinding: Removes an input binding from a mouse input action --- |
---------------------------------------------------------------------------*/
void CaledonEngine::InputAction::RemoveBinding(MouseCode mouse)
{
    m_inputBindings.erase(
        std::remove_if(m_inputBindings.begin(), m_inputBindings.end(),
            [mouse](const InputBinding& binding)
            {
                return binding.m_isMouseButton && binding.m_mouseCode == mouse;
            }),
        m_inputBindings.end());
}

/*------------------------------------------------------------------
| --- ClearBindings: Clears all input bindings from the action --- |
------------------------------------------------------------------*/
void CaledonEngine::InputAction::ClearBindings()
{
    m_inputBindings.clear();
    m_compositeBindings.clear();
}

/*--------------------------------------------------------------------------------------------------------
| --- AddPositiveBinding: Adds a positive binding to a composite binding for a keyboard input action --- |
--------------------------------------------------------------------------------------------------------*/
void CaledonEngine::InputAction::AddPositiveBinding(KeyCode key)
{
    if (m_compositeBindings.empty())
    {
        m_compositeBindings.emplace_back();
    }
    m_compositeBindings[0].m_positiveBindings.emplace_back(key);
}

/*--------------------------------------------------------------------------------------------------------
| --- AddNegativeBinding: Adds a negative binding to a composite binding for a keyboard input action --- |
--------------------------------------------------------------------------------------------------------*/
void CaledonEngine::InputAction::AddNegativeBinding(KeyCode key)
{
    if (m_compositeBindings.empty())
    {
        m_compositeBindings.emplace_back();
    }
    m_compositeBindings[0].m_negativeBindings.emplace_back(key);
}

/*-----------------------------------------------------------------------------------------------------
| --- AddPositiveBinding: Adds a positive binding to a composite binding for a mouse input action --- |
-----------------------------------------------------------------------------------------------------*/
void CaledonEngine::InputAction::AddPositiveBinding(MouseCode mouse)
{
    if (m_compositeBindings.empty())
    {
        m_compositeBindings.emplace_back();
    }
    m_compositeBindings[0].m_positiveBindings.emplace_back(mouse);
}

/*-----------------------------------------------------------------------------------------------------
| --- AddNegativeBinding: Adds a negative binding to a composite binding for a mouse input action --- |
-----------------------------------------------------------------------------------------------------*/
void CaledonEngine::InputAction::AddNegativeBinding(MouseCode mouse)
{
    if (m_compositeBindings.empty())
    {
        m_compositeBindings.emplace_back();
    }
    m_compositeBindings[0].m_negativeBindings.emplace_back(mouse);
}

/*---------------------------------------------------------------------------------------------
| --- Unsubscribe: Unsubscribes all callbacks bound to a specific object instance pointer --- |
---------------------------------------------------------------------------------------------*/
void CaledonEngine::InputAction::Unsubscribe(void* pInstance)
{
    if (!pInstance)
        return;

    auto matchesTarget = [pInstance](const CallbackDelegate& delegate) {
        return delegate.m_pTargetInstance == pInstance;
        };

    std::erase_if(m_onStartedCallbacks, matchesTarget);
    std::erase_if(m_onPerformedCallbacks, matchesTarget);
    std::erase_if(m_onCanceledCallbacks, matchesTarget);
    std::erase_if(m_onValueCallbacks, matchesTarget);
}

/*-------------------------------------------------------------------------
| --- Process: Evaluates action state transitions and fires callbacks --- |
-------------------------------------------------------------------------*/
void CaledonEngine::InputAction::Process(float value)
{
    bool isActive = (value != 0.0f);
    Context context{ this, ActionPhase::kStarted, value };

    if (m_actionType == ActionType::kButton)
    {
        // 1. Pressed Down: Transition from inactive -> active
        if (isActive && !m_wasActive)
        {
            context.m_phase = ActionPhase::kStarted;
            InvokeStartedCallbacks(context);

            context.m_phase = ActionPhase::kPerformed;
            InvokePerformedCallbacks(context);
        }
        // 2. Released: Transition from active -> inactive
        else if (!isActive && m_wasActive)
        {
            context.m_phase = ActionPhase::kCanceled;
            InvokeCanceledCallbacks(context);
        }
    }
    else if (m_actionType == ActionType::kValue)
    {
        // 1. Initial Movement: Value moved off 0.0f
        if (isActive && !m_wasActive)
        {
            context.m_phase = ActionPhase::kStarted;
            InvokeStartedCallbacks(context);
        }
        // 2. Returned to Neutral: Value returned to 0.0f
        else if (!isActive && m_wasActive)
        {
            context.m_phase = ActionPhase::kCanceled;
            InvokeCanceledCallbacks(context);
        }

        // 3. Ongoing Value Update: Fire Performed while active
        if (isActive)
        {
            context.m_phase = ActionPhase::kPerformed;
            InvokePerformedCallbacks(context);
        }

        // 4. Value Callbacks: Fire whenever value is processed (active or inactive)
        InvokeValueCallbacks(context);
    }

    m_wasActive = isActive;
}

/*-------------------------------------------------------
| --- GetName: Returns the name of the input action --- |
-------------------------------------------------------*/
const std::string& CaledonEngine::InputAction::GetName() const
{
    return m_name;
}

/*--------------------------------------------------------------------
| --- GetActionType: Returns the action type of the input action --- |
--------------------------------------------------------------------*/
CaledonEngine::ActionType CaledonEngine::InputAction::GetActionType() const
{
    return m_actionType;
}

/*----------------------------------------------------------------------
| --- GetControlType: Returns the control type of the input action --- |
----------------------------------------------------------------------*/
CaledonEngine::ControlType CaledonEngine::InputAction::GetControlType() const
{
    return m_controlType;
}

/*--------------------------------------------------------------------
| --- GetInitialStateCheck: Returns the initial state check flag --- |
--------------------------------------------------------------------*/
bool CaledonEngine::InputAction::GetInitialStateCheck() const
{
    return m_initialStateCheck;
}

/*------------------------------------------------------------------
| --- IsEnabled: Returns the enabled state of the input action --- |
------------------------------------------------------------------*/
bool CaledonEngine::InputAction::IsEnabled() const
{
    return m_isEnabled;
}

/*--------------------------------------------------------------------
| --- GetInputBindings: Returns the input bindings of the action --- |
--------------------------------------------------------------------*/
const std::vector<CaledonEngine::InputBinding>& CaledonEngine::InputAction::GetInputBindings() const
{
    return m_inputBindings;
}

/*----------------------------------------------------------------------------
| --- GetCompositeBindings: Returns the composite bindings of the action --- |
----------------------------------------------------------------------------*/
const std::vector<CaledonEngine::CompositeBinding>& CaledonEngine::InputAction::GetCompositeBindings() const
{
    return m_compositeBindings;
}


/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*-----------------------------------------------------------------------------
| --- InvokeCallbacks: Executes registered callbacks for a specific event --- |
-----------------------------------------------------------------------------*/
void CaledonEngine::InputAction::InvokeCallbacks(const std::vector<CallbackDelegate>& callbacks, const Context& context)
{
    for (const auto& delegate : callbacks)
    {
        if (delegate.m_callback)
        {
            delegate.m_callback(context);
        }
    }
}

void CaledonEngine::InputAction::InvokeStartedCallbacks(const Context& context) { InvokeCallbacks(m_onStartedCallbacks, context); }
void CaledonEngine::InputAction::InvokePerformedCallbacks(const Context& context) { InvokeCallbacks(m_onPerformedCallbacks, context); }
void CaledonEngine::InputAction::InvokeCanceledCallbacks(const Context& context) { InvokeCallbacks(m_onCanceledCallbacks, context); }
void CaledonEngine::InputAction::InvokeValueCallbacks(const Context& context) { InvokeCallbacks(m_onValueCallbacks, context); }