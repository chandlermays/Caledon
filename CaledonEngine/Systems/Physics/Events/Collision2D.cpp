#include "Collision2D.h"

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Physics/Components/RigidBody2D.h"
#include "Systems/Scene/GameObject.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------------------------
| --- GetGameObject:
---------------------------------------------------------------------------------------*/
CaledonEngine::GameObject* CaledonEngine::Collision2D::GetGameObject() const noexcept
{
	if (m_pRigidBody)
		return m_pRigidBody->GetOwner();

	return m_pCollider ? m_pCollider->GetOwner() : nullptr;
}

/*---------------------------------------------------------------------------------------
| --- GetTransform:
---------------------------------------------------------------------------------------*/
CaledonEngine::Transform* CaledonEngine::Collision2D::GetTransform() const noexcept
{
	GameObject* pGameObject = GetGameObject();
	return pGameObject ? &pGameObject->GetTransform() : nullptr;
}