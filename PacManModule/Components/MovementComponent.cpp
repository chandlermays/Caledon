/*------------------------------
| File: MovementComponent.cpp
| Author: Chandler Mays
------------------------------*/
#include "MovementComponent.h"

#include "CaledonEngine/Systems/Scene/GameObject.h"
#include "CaledonEngine/Systems/Scene/Transform.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------------
| --- Constructor: Constructs the MovementComponent with default values --- |
---------------------------------------------------------------------------*/
PacmanModule::MovementComponent::MovementComponent()
    : Component()
    , m_direction{ 0.0f, 0.0f }
    , m_speed{ 0.0f }
{ }

/*-------------------------------------------------------------
| --- GetTypeName: Returns the type name of the Component --- |
-------------------------------------------------------------*/
const std::string& PacmanModule::MovementComponent::GetTypeName() const
{
    static const std::string typeName = "MovementComponent";
    return typeName;
}

/*------------------------------------------------------------------------
| --- Update:
------------------------------------------------------------------------*/
void PacmanModule::MovementComponent::Update(float deltaTime)
{
    if (m_speed == 0.0f || m_direction == CaledonEngine::Vector2f::Zero())
        return;

    if (CaledonEngine::GameObject* pOwner = GetOwner())
    {
        CaledonEngine::Transform& transform = pOwner->GetTransform();
        CaledonEngine::Vector2f currentPos = transform.GetPosition();

        // Move the transform position
        currentPos += m_direction * m_speed * deltaTime;
        transform.SetPosition(currentPos);
    }
}

/*------------------------------------------------------------------------
| --- SetDirection:
------------------------------------------------------------------------*/
void PacmanModule::MovementComponent::SetDirection(const CaledonEngine::Vector2f direction)
{
    m_direction = direction;
}

/*------------------------------------------------------------------------
| --- GetDirection:
------------------------------------------------------------------------*/
const CaledonEngine::Vector2f& PacmanModule::MovementComponent::GetDirection() const
{
    return m_direction;
}

/*------------------------------------------------------------------------
| --- SetSpeed:
------------------------------------------------------------------------*/
void PacmanModule::MovementComponent::SetSpeed(float speed)
{
    m_speed = speed;
}

/*------------------------------------------------------------------------
| --- GetSped:
------------------------------------------------------------------------*/
float PacmanModule::MovementComponent::GetSpeed() const
{
    return m_speed;
}

/*------------------------------------------------------------------------
| --- Stop:
------------------------------------------------------------------------*/
void PacmanModule::MovementComponent::Stop()
{
    m_direction = CaledonEngine::Vector2f::Zero();
}