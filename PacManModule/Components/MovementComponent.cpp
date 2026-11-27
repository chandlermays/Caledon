/*------------------------------
| File: MovementComponent.cpp
| Author: Chandler Mays
------------------------------*/
#include "MovementComponent.h"

#include "CaledonEngine/Systems/Physics/Components/RigidBody2D.h"
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
{
}

/*-------------------------------------------------------------
| --- GetTypeName: Returns the type name of the Component --- |
-------------------------------------------------------------*/
const std::string& PacmanModule::MovementComponent::GetTypeName() const
{
    static const std::string typeName = "MovementComponent";
    return typeName;
}

/*---------------------------------------------------------------------------
| --- Update: Moves the owner (through its RigidBody2D when it has one) --- |
---------------------------------------------------------------------------*/
void PacmanModule::MovementComponent::Update(float deltaTime)
{
    // With a RigidBody2D the physics system moves the object (and stops it at colliders), so drive its velocity instead of the Transform
    if (CaledonEngine::RigidBody2D* pBody = GetComponent<CaledonEngine::RigidBody2D>())
    {
        pBody->SetLinearVelocity(m_direction * m_speed);
        return;
    }

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

/*-----------------------------------------------------
| --- SetDirection: Sets the direction to move in --- |
-----------------------------------------------------*/
void PacmanModule::MovementComponent::SetDirection(const CaledonEngine::Vector2f direction)
{
    m_direction = direction;
}

/*--------------------------------------------------------
| --- GetDirection: Returns the direction to move in --- |
--------------------------------------------------------*/
const CaledonEngine::Vector2f& PacmanModule::MovementComponent::GetDirection() const
{
    return m_direction;
}

/*---------------------------------------------
| --- SetSpeed: Sets the speed to move at --- |
---------------------------------------------*/
void PacmanModule::MovementComponent::SetSpeed(float speed)
{
    m_speed = speed;
}

/*------------------------------------------------
| --- GetSpeed: Returns the speed to move at --- |
------------------------------------------------*/
float PacmanModule::MovementComponent::GetSpeed() const
{
    return m_speed;
}

/*------------------------------------------------------
| --- Stop: Stops moving by clearing the direction --- |
------------------------------------------------------*/
void PacmanModule::MovementComponent::Stop()
{
    m_direction = CaledonEngine::Vector2f::Zero();
}