/*------------------------------
| File: Input.cpp
| Author: Chandler Mays
------------------------------*/
#include "Input.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------
| --- Constructor: Constructs the Input with default values --- |
---------------------------------------------------------------*/
CaledonEngine::Input::Input()
    : m_mousePositionX{ 0.0f }
    , m_mousePositionY{ 0.0f }
    , m_mouseWheelDelta{ 0 }
{
    // Initialize all keys to the released state
    for (int i = 0; i < static_cast<int>(KeyCode::kNumKeys); ++i)
    {
        m_keyStates[static_cast<KeyCode>(i)] = false;
        m_prevKeyStates[static_cast<KeyCode>(i)] = false;
    }

    // Initialize all mouse buttons to the released state
    for (int i = 0; i < static_cast<int>(MouseCode::kNumMouseButtons); ++i)
    {
        m_mouseStates[static_cast<MouseCode>(i)] = false;
        m_prevMouseStates[static_cast<MouseCode>(i)] = false;
    }
}

/*-----------------------------------------------------------------------------
| --- IsKeyHeld: Returns true if the specified key is currently held down --- |
-----------------------------------------------------------------------------*/
bool CaledonEngine::Input::IsKeyHeld(KeyCode key) const
{
    auto it = m_keyStates.find(key);
    if (it != m_keyStates.end())
    {
        return it->second;
    }
    return false;
}

/*--------------------------------------------------------------------------------
| --- IsKeyPressed: Returns true if the specified key was pressed this frame --- |
--------------------------------------------------------------------------------*/
bool CaledonEngine::Input::IsKeyPressed(KeyCode key) const
{
    auto currentIt = m_keyStates.find(key);
    auto prevIt = m_prevKeyStates.find(key);

    if (currentIt != m_keyStates.end() && prevIt != m_prevKeyStates.end())
    {
        return (currentIt->second && !prevIt->second);
    }
    return false;
}

/*----------------------------------------------------------------------------------
| --- IsKeyReleased: Returns true if the specified key was released this frame --- |
----------------------------------------------------------------------------------*/
bool CaledonEngine::Input::IsKeyReleased(KeyCode key) const
{
    auto currentIt = m_keyStates.find(key);
    auto prevIt = m_prevKeyStates.find(key);

    if (currentIt != m_keyStates.end() && prevIt != m_prevKeyStates.end())
    {
        return (!currentIt->second && prevIt->second);
    }
    return false;
}

/*----------------------------------------------------------------------------------------------
| --- IsMouseButtonHeld: Returns true if the specified mouse button is currently held down --- |
----------------------------------------------------------------------------------------------*/
bool CaledonEngine::Input::IsMouseButtonHeld(MouseCode button) const
{
    auto it = m_mouseStates.find(button);
    if (it != m_mouseStates.end())
    {
        return it->second;
    }
    return false;
}

/*-------------------------------------------------------------------------------------------------
| --- IsMouseButtonPressed: Returns true if the specified mouse button was pressed this frame --- |
-------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Input::IsMouseButtonPressed(MouseCode button) const
{
    auto currentIt = m_mouseStates.find(button);
    auto prevIt = m_prevMouseStates.find(button);

    if (currentIt != m_mouseStates.end() && prevIt != m_prevMouseStates.end())
    {
        return (currentIt->second && !prevIt->second);
    }
    return false;
}

/*---------------------------------------------------------------------------------------------------
| --- IsMouseButtonReleased: Returns true if the specified mouse button was released this frame --- |
---------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Input::IsMouseButtonReleased(MouseCode button) const
{
    auto currentIt = m_mouseStates.find(button);
    auto prevIt = m_prevMouseStates.find(button);

    if (currentIt != m_mouseStates.end() && prevIt != m_prevMouseStates.end())
    {
        return (!currentIt->second && prevIt->second);
    }
    return false;
}

/*--------------------------------------------------------------
| --- GetMousePosition: Returns the current mouse position --- |
--------------------------------------------------------------*/
void CaledonEngine::Input::GetMousePosition(float& x, float& y) const
{
    x = m_mousePositionX;
    y = m_mousePositionY;
}

/*----------------------------------------------------------------------------
| --- GetMouseWheelDelta: Returns the mouse wheel delta since last frame --- |
----------------------------------------------------------------------------*/
float CaledonEngine::Input::GetMouseWheelDelta() const
{
    return m_mouseWheelDelta;
}