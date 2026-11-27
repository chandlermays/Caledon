/*------------------------------
| File: Scene.cpp
| Author: Chandler Mays
------------------------------*/
#include "Scene.h"

#include "Systems/Scene/Component.h"
#include "Systems/Scene/GameObject.h"
#include "Systems/Scene/Transform.h"

#include <algorithm>
#include <unordered_set>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------
| --- Constructor: Constructs the Scene with default values --- |
---------------------------------------------------------------*/
CaledonEngine::Scene::Scene()
	: m_name{ "Scene" }
	, m_isActive{ true }
	, m_isDirty{ false }
	, m_isLoaded{ false }
	, m_filePath{ "" }
	, m_pPhysicsScene2D{ nullptr }
	, m_isUpdating{ false }
{
}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::Scene::~Scene()
{
	Clear();
}

/*----------------------------------------------------------------
| --- Initialize: Initializes this Scene and its GameObjects --- |
----------------------------------------------------------------*/
bool CaledonEngine::Scene::Initialize()
{
	if (m_isLoaded)
		return true;

	bool success = true;
	for (const auto& pObject : m_gameObjects)
	{
		if (pObject != nullptr && !pObject->Initialize())
		{
			success = false;
		}
	}
	m_isLoaded = success;
	return success;
}

/*---------------------------------------------------------------------------------------------------
| --- Update: Updates this Scene and its GameObjects. It walks a copy of the list, and anything --- |
| ---         added meanwhile waits in a queue, so Components can spawn, reparent and destroy   --- |
---------------------------------------------------------------------------------------------------*/
void CaledonEngine::Scene::Update(float deltaTime)
{
	std::vector<GameObject*> gameObjects;
	gameObjects.reserve(m_gameObjects.size());
	for (const auto& pObject : m_gameObjects)
	{
		gameObjects.push_back(pObject.get());
	}

	m_isUpdating = true;

	for (GameObject* pObject : gameObjects)
	{
		if (pObject != nullptr && pObject->IsActive())
		{
			pObject->Update(deltaTime);
		}
	}

	m_isUpdating = false;
}

/*-------------------------------------------------
| --- GetName: Returns the name of this Scene --- |
-------------------------------------------------*/
const std::string& CaledonEngine::Scene::GetName() const
{
	return m_name;
}

/*----------------------------------------------
| --- SetName: Sets the name of this Scene --- |
----------------------------------------------*/
void CaledonEngine::Scene::SetName(const std::string& name)
{
	m_name = name;
}

/*---------------------------------------------------------------
| --- IsActive: Returns whether this Scene is active or not --- |
---------------------------------------------------------------*/
bool CaledonEngine::Scene::IsActive() const
{
	return m_isActive;
}

/*-------------------------------------------------------------
| --- SetActive: Sets whether this Scene is active or not --- |
-------------------------------------------------------------*/
void CaledonEngine::Scene::SetActive(bool isActive)
{
	m_isActive = isActive;
}

/*-----------------------------------------------------------------
| --- IsDirty: Returns whether this Scene has unsaved changes --- |
-----------------------------------------------------------------*/
bool CaledonEngine::Scene::IsDirty() const
{
	return m_isDirty;
}

/*---------------------------------------------------------------
| --- SetDirty: Sets whether this Scene has unsaved changes --- |
---------------------------------------------------------------*/
void CaledonEngine::Scene::SetDirty(bool dirty)
{
	m_isDirty = dirty;
}

/*----------------------------------------------
| --- MarkClean: Marks this Scene as saved --- |
----------------------------------------------*/
void CaledonEngine::Scene::MarkClean()
{
	m_isDirty = false;
}

/*------------------------------------------------
| --- IsValid: Checks if this Scene is valid --- |
------------------------------------------------*/
bool CaledonEngine::Scene::IsValid() const
{
	return m_isLoaded && !m_gameObjects.empty();
}

/*-----------------------------------------------------------------------
| --- GetFilePath: Returns the file path this Scene was loaded from --- |
-----------------------------------------------------------------------*/
const std::string& CaledonEngine::Scene::GetFilePath() const
{
	return m_filePath;
}

/*----------------------------------------------------------------
| --- SetFilePath: Sets the file path this Scene is saved to --- |
----------------------------------------------------------------*/
void CaledonEngine::Scene::SetFilePath(const std::string& filePath)
{
	m_filePath = filePath;
}

/*-----------------------------------------------------------------------
| --- GetPhysicsScene2D: Returns the PhysicsScene2D this Scene uses --- |
-----------------------------------------------------------------------*/
CaledonEngine::PhysicsScene2D* CaledonEngine::Scene::GetPhysicsScene2D() const noexcept
{
	return m_pPhysicsScene2D;
}

/*--------------------------------------------------------------------
| --- SetPhysicsScene2D: Sets the PhysicsScene2D this Scene uses --- |
--------------------------------------------------------------------*/
void CaledonEngine::Scene::SetPhysicsScene2D(PhysicsScene2D* pPhysicsScene)
{
	m_pPhysicsScene2D = pPhysicsScene;
}

/*------------------------------------------------------------------------------------------------
| --- AddGameObject: Adds a GameObject at the root (or under pParent). While the Scene is    --- |
| ---                updating it is queued and joins at the end of the frame; a loaded Scene --- |
| ---                initializes it as it joins. The pointer it returns is valid right away  --- |
------------------------------------------------------------------------------------------------*/
CaledonEngine::GameObject* CaledonEngine::Scene::AddGameObject(std::unique_ptr<GameObject> pGameObject, GameObject* pParent)
{
	if (!pGameObject || (pParent != nullptr && pParent->GetScene() != this))
		return nullptr;

	GameObject* pRaw = pGameObject.get();

	if (m_isUpdating)
	{
		pRaw->SetScene(this);
		pRaw->m_pParent = pParent;
		m_pendingAdds.push_back(PendingAdd{ std::move(pGameObject), pParent });
		QueueMarkedInSubtree(pRaw);
		return pRaw;
	}

	if (pParent != nullptr)
	{
		pParent->AttachChild(std::move(pGameObject));
	}
	else
	{
		AttachGameObject(std::move(pGameObject));
	}

	// Destroy() on a GameObject outside any Scene only marked it; queue it now, so it doesn't linger skipped forever
	QueueMarkedInSubtree(pRaw);

	if (m_isLoaded && !pRaw->IsPendingDestroy())
	{
		pRaw->Initialize();
	}
	return pRaw;
}

/*----------------------------------------------------------------------------------------------
| --- GetGameObjects: Returns a vector of unique pointers to the GameObjects in this Scene --- |
----------------------------------------------------------------------------------------------*/
const std::vector<std::unique_ptr<CaledonEngine::GameObject>>& CaledonEngine::Scene::GetGameObjects() const
{
	return m_gameObjects;
}

/*-----------------------------------------------------------------------------
| --- Clear: Clears all GameObjects (and anything queued) from this Scene --- |
-----------------------------------------------------------------------------*/
void CaledonEngine::Scene::Clear()
{
	m_pendingDestroyComponents.clear();
	m_pendingDestroyObjects.clear();
	m_pendingAdds.clear();
	m_gameObjects.clear();
}

/*--------------------------------------------------------------------------------------------------------
| --- FlushPendingChanges: Runs once per frame after Update (and also while the game isn't playing). --- |
| ---                      1. Adds the GameObjects created during Update                             --- |
| ---                      2. Destroys marked Components first, so colliders unregister from physics --- |
| ---                      3. Destroys marked GameObjects; a marked child of a marked parent is      --- |
| ---                         skipped, because destroying the parent destroys it too                 --- |
--------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Scene::FlushPendingChanges()
{
	// 1. Adds (swapped out first, since initializing one may queue more for next frame)
	std::vector<PendingAdd> pendingAdds = std::move(m_pendingAdds);
	m_pendingAdds.clear();

	for (PendingAdd& pendingAdd : pendingAdds)
	{
		GameObject* pRaw = pendingAdd.pGameObject.get();

		if (pendingAdd.pParent != nullptr)
		{
			pendingAdd.pParent->AttachChild(std::move(pendingAdd.pGameObject));
		}
		else
		{
			AttachGameObject(std::move(pendingAdd.pGameObject));
		}

		if (m_isLoaded && !pRaw->IsPendingDestroy())
		{
			pRaw->Initialize();
		}
	}

	// 2. Components, unless their GameObject is going too (it takes them with it in step 3)
	std::vector<Component*> pendingComponents = std::move(m_pendingDestroyComponents);
	m_pendingDestroyComponents.clear();

	std::unordered_set<Component*> seenComponents;
	for (Component* pComponent : pendingComponents)
	{
		if (!seenComponents.insert(pComponent).second)
			continue;

		GameObject* pOwner = pComponent->GetOwner();
		if (pOwner != nullptr && !pOwner->IsPendingDestroy())
		{
			pOwner->ReleaseComponent(pComponent);
		}
	}

	// 3. GameObjects: drop duplicates and anything under another marked GameObject, then free each from its owner
	std::vector<GameObject*> pendingObjects = std::move(m_pendingDestroyObjects);
	m_pendingDestroyObjects.clear();

	std::unordered_set<GameObject*> marked{ pendingObjects.begin(), pendingObjects.end() };

	// Decide which ones to free before freeing any, since a freed parent takes its marked children with it
	std::vector<GameObject*> topMost;
	for (GameObject* pObject : pendingObjects)
	{
		bool isUnderMarkedParent = false;
		for (GameObject* pParent = pObject->GetParent(); pParent != nullptr; pParent = pParent->GetParent())
		{
			if (marked.count(pParent) > 0)
			{
				isUnderMarkedParent = true;
				break;
			}
		}

		if (!isUnderMarkedParent && std::find(topMost.begin(), topMost.end(), pObject) == topMost.end())
		{
			topMost.push_back(pObject);
		}
	}

	for (GameObject* pObject : topMost)
	{
		std::unique_ptr<GameObject> pDestroyed = pObject->GetParent() ? pObject->GetParent()->ReleaseChild(pObject) : ReleaseGameObject(pObject);
		pDestroyed.reset();
	}
}

/*-------------------------------------------------------------------------------------------
| --- HasPendingChanges: Returns whether anything is waiting for the end-of-frame flush --- |
-------------------------------------------------------------------------------------------*/
bool CaledonEngine::Scene::HasPendingChanges() const noexcept
{
	return !m_pendingAdds.empty() || !m_pendingDestroyComponents.empty() || !m_pendingDestroyObjects.empty();
}

/*------------------------------------------------------------------
| --- IsLoaded: Returns whether the Scene has been initialized --- |
------------------------------------------------------------------*/
bool CaledonEngine::Scene::IsLoaded() const noexcept
{
	return m_isLoaded;
}

/*--------------------------------------------------------------------------------------------------------------
| --- Contains: Returns whether the GameObject is in this Scene's hierarchy (not still waiting in a queue) --- |
--------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Scene::Contains(const GameObject* pGameObject) const
{
	if (pGameObject == nullptr || pGameObject->GetScene() != this)
		return false;

	// Climb to the root, checking each step is really owned by the one above (a queued child only points up)
	const GameObject* pCurrent = pGameObject;
	while (pCurrent->GetParent() != nullptr)
	{
		const GameObject* pParent = pCurrent->GetParent();
		const auto& children = pParent->GetChildren();
		const bool isOwned = std::any_of(children.begin(), children.end(),
			[pCurrent](const std::unique_ptr<GameObject>& p) { return p.get() == pCurrent; });

		if (!isOwned)
			return false;

		pCurrent = pParent;
	}

	return std::any_of(m_gameObjects.begin(), m_gameObjects.end(),
		[pCurrent](const std::unique_ptr<GameObject>& p) { return p.get() == pCurrent; });
}

/*------------------------------------------------------------------------------------------------------
| --- FindGameObject: Finds a GameObject (roots, children and queued ones) by its ID (brute force) --- |
------------------------------------------------------------------------------------------------------*/
CaledonEngine::GameObject* CaledonEngine::Scene::FindGameObject(std::uint64_t id) const
{
	if (id == 0)
		return nullptr;

	std::vector<GameObject*> stack;
	for (const auto& pObject : m_gameObjects)
	{
		stack.push_back(pObject.get());
	}
	for (const PendingAdd& pendingAdd : m_pendingAdds)
	{
		stack.push_back(pendingAdd.pGameObject.get());
	}

	while (!stack.empty())
	{
		GameObject* pObject = stack.back();
		stack.pop_back();

		if (pObject == nullptr)
			continue;

		if (pObject->GetID() == id)
			return pObject;

		for (const auto& pChild : pObject->GetChildren())
		{
			stack.push_back(pChild.get());
		}
	}

	return nullptr;
}

/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*------------------------------------------------------------------------------------
| --- AttachGameObject: Takes ownership of a root GameObject (no initialization) --- |
------------------------------------------------------------------------------------*/
void CaledonEngine::Scene::AttachGameObject(std::unique_ptr<GameObject> pGameObject)
{
	if (!pGameObject)
		return;

	pGameObject->m_pParent = nullptr;
	pGameObject->SetScene(this);
	pGameObject->GetTransform().MarkWorldDirty();
	m_gameObjects.emplace_back(std::move(pGameObject));
}

/*-----------------------------------------------------------------------------------------------------
| --- ReleaseGameObject: Hands ownership of a root GameObject to the caller without destroying it --- |
-----------------------------------------------------------------------------------------------------*/
std::unique_ptr<CaledonEngine::GameObject> CaledonEngine::Scene::ReleaseGameObject(GameObject* pGameObject)
{
	auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(),
		[pGameObject](const std::unique_ptr<GameObject>& p) { return p.get() == pGameObject; });

	if (it == m_gameObjects.end())
		return nullptr;

	std::unique_ptr<GameObject> pReleased = std::move(*it);
	m_gameObjects.erase(it);
	return pReleased;
}

/*-------------------------------------------------------------------------------------------------------
| --- RetargetPendingAdd: Changes where a queued GameObject will be added; false if it isn't queued --- |
-------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Scene::RetargetPendingAdd(GameObject* pGameObject, GameObject* pNewParent)
{
	for (PendingAdd& pendingAdd : m_pendingAdds)
	{
		if (pendingAdd.pGameObject.get() == pGameObject)
		{
			pendingAdd.pParent = pNewParent;
			return true;
		}
	}
	return false;
}

/*-----------------------------------------------------------------
| --- QueueDestroy: Queues a GameObject marked with Destroy() --- |
-----------------------------------------------------------------*/
void CaledonEngine::Scene::QueueDestroy(GameObject* pGameObject)
{
	if (pGameObject != nullptr)
	{
		m_pendingDestroyObjects.push_back(pGameObject);
	}
}

/*------------------------------------------------------------------------
| --- QueueDestroy: Queues a Component marked with RemoveComponent() --- |
------------------------------------------------------------------------*/
void CaledonEngine::Scene::QueueDestroy(Component* pComponent)
{
	if (pComponent != nullptr)
	{
		m_pendingDestroyComponents.push_back(pComponent);
	}
}

/*-----------------------------------------------------------------------------------------------------
| --- QueueMarkedInSubtree: Queues GameObjects that were marked with Destroy() before they joined --- |
| ---                       this Scene (a GameObject outside any Scene can only be marked)        --- |
-----------------------------------------------------------------------------------------------------*/
void CaledonEngine::Scene::QueueMarkedInSubtree(GameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return;

	if (pGameObject->m_isPendingDestroy)
	{
		QueueDestroy(pGameObject);
		return;													// Its children go with it
	}

	for (const auto& pChild : pGameObject->m_children)
	{
		QueueMarkedInSubtree(pChild.get());
	}
}
