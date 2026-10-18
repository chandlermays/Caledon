/*------------------------------
| File: ColliderHit2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ColliderHit2D.h"

#include "Systems/Physics/Components/Collider2D.h"
#include "Systems/Scene/GameObject.h"
#include "Systems/Scene/Transform.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------------------------
| --- GetTransform:
---------------------------------------------------------------------------------------*/
CaledonEngine::Transform* CaledonEngine::ColliderHit2D::GetTransform() const noexcept
{
	if (!pCollider)
		return nullptr;

	GameObject* pOwner = pCollider->GetOwner();
	return pOwner ? &pOwner->GetTransform() : nullptr;
}