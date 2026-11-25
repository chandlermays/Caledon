/*------------------------------
| File: GameObject.cpp
| Author: Chandler Mays
------------------------------*/
#include "GameObject.h"

#include "Systems/Scene/Component.h"
#include "Systems/Scene/Scene.h"
#include "Systems/Scene/Transform.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <random>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*--------------------------------------------------------------------
| --- Constructor: Constructs the GameObject with default values --- |
--------------------------------------------------------------------*/
CaledonEngine::GameObject::GameObject()
	: m_id{ GenerateID() }
	, m_pScene{ nullptr }
	, m_pParent{ nullptr }
	, m_name{ "GameObject" }
	, m_tag{ "Untagged" }
	, m_layer{ 0 }
	, m_isActive{ true }
	, m_isPendingDestroy{ false }
	, m_pTransform{ nullptr }
{
	// Create a new Transform component and add it to the GameObject
	auto pTransform = std::make_unique<Transform>();
	m_pTransform = pTransform.get();
	AddComponent(std::move(pTransform));
}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::GameObject::~GameObject()
{
	DestroyImmediate();
}

/*--------------------------------------------------------------------
| --- Initialize: Initializes this GameObject and its components --- |
--------------------------------------------------------------------*/
bool CaledonEngine::GameObject::Initialize()
{
	bool success = true;
	for (const auto& pComponent : m_components)
	{
		if (pComponent != nullptr && !pComponent->Initialize())
		{
			success = false;
		}
	}

	for (const auto& pChild : m_children)
	{
		if (pChild != nullptr && !pChild->Initialize())
		{
			success = false;
		}
	}

	return success;
}

/*---------------------------------------------------------------------------------------------------
| --- Update: Updates this GameObject and its components. It walks copies of the lists, so a    --- |
| ---         Component may add Components, reparent or destroy objects without invalidating it --- |
---------------------------------------------------------------------------------------------------*/
void CaledonEngine::GameObject::Update(float deltaTime)
{
	if (!m_isActive || IsPendingDestroy())
		return;

	std::vector<Component*> components;
	components.reserve(m_components.size());
	for (const auto& pComponent : m_components)
	{
		components.push_back(pComponent.get());
	}

	for (Component* pComponent : components)
	{
		if (pComponent != nullptr && pComponent->IsActive() && !pComponent->m_isPendingDestroy)
		{
			pComponent->Update(deltaTime);
		}
	}

	std::vector<GameObject*> children;
	children.reserve(m_children.size());
	for (const auto& pChild : m_children)
	{
		children.push_back(pChild.get());
	}

	for (GameObject* pChild : children)
	{
		if (pChild != nullptr && pChild->IsActive())
		{
			pChild->Update(deltaTime);
		}
	}
}

/*-------------------------------------------------------------------------
| --- GetID: Returns the stable, unique identifier of this GameObject --- |
-------------------------------------------------------------------------*/
std::uint64_t CaledonEngine::GameObject::GetID() const noexcept
{
	return m_id;
}

/*------------------------------------------------------------------------------
| --- SetID: Sets the identifier (used when loading a scene; 0 is ignored) --- |
------------------------------------------------------------------------------*/
void CaledonEngine::GameObject::SetID(std::uint64_t id) noexcept
{
	if (id != 0)
	{
		m_id = id;
	}
}

/*------------------------------------------------------------------------
| --- GetScene: Returns a pointer to the Scene this GameObject is in --- |
------------------------------------------------------------------------*/
CaledonEngine::Scene* CaledonEngine::GameObject::GetScene() const
{
	return m_pScene;
}

/*--------------------------------------------------------
| --- SetScene: Sets the Scene this GameObject is in --- |
--------------------------------------------------------*/
void CaledonEngine::GameObject::SetScene(Scene* pScene)
{
	m_pScene = pScene;
	for (const auto& pChild : m_children)
	{
		pChild->SetScene(pScene);
	}
}

/*------------------------------------------------------------------
| --- GetParent: Returns a pointer to this GameObject's parent --- |
------------------------------------------------------------------*/
CaledonEngine::GameObject* CaledonEngine::GameObject::GetParent() const
{
	return m_pParent;
}

/*------------------------------------------------------------------------------------------------------
| --- SetParent: Moves this GameObject under a new parent (nullptr = scene root). Ownership moves  --- |
| ---            with it, nothing is destroyed. With worldPositionStays the world pose is kept and --- |
| ---            the local pose is recalculated; otherwise the local pose is kept                  --- |
| ---            Fails if the new parent is this GameObject or one of its children, or is in a     --- |
| ---            different Scene                                                                   --- |
------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::GameObject::SetParent(GameObject* pNewParent, bool worldPositionStays)
{
	if (pNewParent == m_pParent)
		return true;

	if (pNewParent == this || (pNewParent != nullptr && IsAncestorOf(pNewParent)))
		return false;

	if (pNewParent != nullptr && pNewParent->m_pScene != m_pScene)
		return false;

	// A GameObject can only become a root inside a Scene
	if (pNewParent == nullptr && m_pScene == nullptr)
		return false;

	// A GameObject added during the Scene's Update is not in the hierarchy yet; just change where it will go
	if (m_pScene != nullptr && m_pScene->RetargetPendingAdd(this, pNewParent))
	{
		m_pParent = pNewParent;
		GetTransform().MarkWorldDirty();
		return true;
	}

	// Saved before moving, because the world pose is worked out from the parent
	const Vector2f worldPosition = GetTransform().GetPosition();
	const float worldRotation = GetTransform().GetRotation();
	const Vector2f worldScale = GetTransform().GetScale();

	std::unique_ptr<GameObject> pSelf;
	if (m_pParent != nullptr)
	{
		pSelf = m_pParent->ReleaseChild(this);
	}
	else if (m_pScene != nullptr)
	{
		pSelf = m_pScene->ReleaseGameObject(this);
	}

	// Without an owner to take it from (a loose GameObject held by a unique_ptr), there is nothing to move
	if (!pSelf)
		return false;

	if (pNewParent != nullptr)
	{
		pNewParent->AttachChild(std::move(pSelf));
	}
	else
	{
		m_pScene->AttachGameObject(std::move(pSelf));
	}

	if (worldPositionStays)
	{
		Transform& transform = GetTransform();
		transform.SetPosition(worldPosition);
		transform.SetRotation(worldRotation);
		transform.SetScale(worldScale);
	}

	return true;
}

/*-------------------------------------------------------------------------------------------------------
| --- IsAncestorOf: Returns whether this GameObject is a parent, grandparent, etc. of the given one --- |
-------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::GameObject::IsAncestorOf(const GameObject* pGameObject) const
{
	for (const GameObject* pCurrent = pGameObject ? pGameObject->m_pParent : nullptr; pCurrent != nullptr; pCurrent = pCurrent->m_pParent)
	{
		if (pCurrent == this)
			return true;
	}
	return false;
}

/*---------------------------------------------------------------------------------
| --- GetChildren: Returns a vector of pointers to this GameObject's children --- |
---------------------------------------------------------------------------------*/
const std::vector<std::unique_ptr<CaledonEngine::GameObject>>& CaledonEngine::GameObject::GetChildren() const
{
	return m_children;
}

/*----------------------------------------------------------------------------------------------------
| --- AddChild: Adds a new child to this GameObject. While the Scene is updating it is queued    --- |
| ---           and added at the end of the frame. In a loaded Scene it is initialized on adding --- |
----------------------------------------------------------------------------------------------------*/
CaledonEngine::GameObject* CaledonEngine::GameObject::AddChild(std::unique_ptr<GameObject> pChild)
{
	if (!pChild || pChild.get() == this)
		return nullptr;

	if (m_pScene != nullptr)
		return m_pScene->AddGameObject(std::move(pChild), this);

	GameObject* pRaw = pChild.get();
	AttachChild(std::move(pChild));
	return pRaw;
}

/*----------------------------------------------------------------------------------------------------
| --- DetachChildren: Moves every child to the scene root, keeping their world poses (Unity's    --- |
| ---                 DetachChildren). Detaching one child is SetParent(nullptr). Needs a Scene, --- |
| ---                 since a root can only live in one                                          --- |
----------------------------------------------------------------------------------------------------*/
void CaledonEngine::GameObject::DetachChildren()
{
	std::vector<GameObject*> children;
	children.reserve(m_children.size());
	for (const auto& pChild : m_children)
	{
		children.push_back(pChild.get());
	}

	for (GameObject* pChild : children)
	{
		pChild->SetParent(nullptr);
	}
}

/*------------------------------------------------------
| --- GetName: Returns the name of this GameObject --- |
------------------------------------------------------*/
const std::string& CaledonEngine::GameObject::GetName() const
{
	return m_name;
}

/*---------------------------------------------------
| --- SetName: Sets the name of this GameObject --- |
---------------------------------------------------*/
void CaledonEngine::GameObject::SetName(const std::string& name)
{
	m_name = name;
}

/*---------------------------------------------------
| --- GetTag: Returns the tag of the GameObject --- |
---------------------------------------------------*/
const std::string& CaledonEngine::GameObject::GetTag() const
{
	return m_tag;
}

/*-------------------------------------------------
| --- SetTag: Sets the tag of this GameObject --- |
-------------------------------------------------*/
void CaledonEngine::GameObject::SetTag(const std::string& tag)
{
	m_tag = tag;
}

/*----------------------------------------------------------------
| --- GetLayer: Returns the physics layer of this GameObject --- |
----------------------------------------------------------------*/
int CaledonEngine::GameObject::GetLayer() const noexcept
{
	return m_layer;
}

/*--------------------------------------------------------------------
| --- IsActive: Returns whether this GameObject is active or not --- |
--------------------------------------------------------------------*/
void CaledonEngine::GameObject::SetLayer(int layer) noexcept
{
	m_layer = std::clamp(layer, 0, 31);
}

/*--------------------------------------------------------------------
| --- IsActive: Returns whether this GameObject is active or not --- |
--------------------------------------------------------------------*/
bool CaledonEngine::GameObject::IsActive() const
{
	return m_isActive;
}

/*------------------------------------------------------------------
| --- SetActive: Sets whether this GameObject is active or not --- |
------------------------------------------------------------------*/
void CaledonEngine::GameObject::SetActive(bool isActive)
{
	m_isActive = isActive;
}

/*----------------------------------------------------------------
| --- GetTransform: Returns the Transform of this GameObject --- |
----------------------------------------------------------------*/
const CaledonEngine::Transform& CaledonEngine::GameObject::GetTransform() const
{
	assert(m_pTransform && "GameObject::GetTransform called on a destroyed GameObject.");
	return *m_pTransform;
}

/*----------------------------------------------------------------
| --- GetTransform: Returns the Transform of this GameObject --- |
----------------------------------------------------------------*/
CaledonEngine::Transform& CaledonEngine::GameObject::GetTransform()
{
	assert(m_pTransform && "GameObject::GetTransform called on a destroyed GameObject.");
	return *m_pTransform;
}

/*------------------------------------------------------------------------------------------------------
| --- GetAllComponents: Returns a vector of pointers to all Components attached to this GameObject --- |
------------------------------------------------------------------------------------------------------*/
const std::vector<std::unique_ptr<CaledonEngine::Component>>& CaledonEngine::GameObject::GetAllComponents() const
{
	return m_components;
}

/*-------------------------------------------------------------------------------------------------
| --- AddComponent: Adds a component to this GameObject. When the GameObject is already in a  --- |
| ---               loaded Scene the Component is initialized now (e.g. a collider registers) --- |
-------------------------------------------------------------------------------------------------*/
void CaledonEngine::GameObject::AddComponent(std::unique_ptr<Component> pComponent)
{
	if (!pComponent)
		return;

	pComponent->SetOwner(this);
	Component* pRaw = pComponent.get();
	m_components.emplace_back(std::move(pComponent));

	if (m_pScene != nullptr && m_pScene->IsLoaded() && m_pScene->Contains(this))
	{
		pRaw->Initialize();
	}
}

/*----------------------------------------------------------------------------------------------------
| --- RemoveComponent: Marks a Component for removal at the end of the frame. It stays valid     --- |
| ---                  until then but no longer updates or draws. The Transform can't be removed --- |
| ---                  A GameObject that is not in a Scene removes it right away                 --- |
----------------------------------------------------------------------------------------------------*/
void CaledonEngine::GameObject::RemoveComponent(Component* pComponent)
{
	if (pComponent == nullptr || pComponent == m_pTransform || pComponent->m_pOwner != this || pComponent->m_isPendingDestroy)
		return;

	if (m_pScene == nullptr)
	{
		ReleaseComponent(pComponent);
		return;
	}

	pComponent->m_isPendingDestroy = true;
	m_pScene->QueueDestroy(pComponent);
}

/*----------------------------------------------------------------------------------------------------
| --- Destroy: Marks this GameObject (and so its children) for destruction at the end of the     --- |
| ---          frame. It stays valid until then but is skipped by Update, rendering and the      --- |
| ---          Hierarchy. A GameObject that is not in a Scene is only marked; its owner frees it --- |
----------------------------------------------------------------------------------------------------*/
void CaledonEngine::GameObject::Destroy()
{
	if (m_isPendingDestroy)
		return;

	m_isPendingDestroy = true;

	if (m_pScene != nullptr)
	{
		m_pScene->QueueDestroy(this);
	}
}

/*----------------------------------------------------------------------------------------------------
| --- IsPendingDestroy: Returns whether this GameObject (or a parent) is waiting to be destroyed --- |
----------------------------------------------------------------------------------------------------*/
bool CaledonEngine::GameObject::IsPendingDestroy() const noexcept
{
	for (const GameObject* pCurrent = this; pCurrent != nullptr; pCurrent = pCurrent->m_pParent)
	{
		if (pCurrent->m_isPendingDestroy)
			return true;
	}
	return false;
}

/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*-------------------------------------------------------------------------------------------------
| --- GenerateID: Returns a new random, non-zero 64-bit identifier. With 2^64 values, two IDs --- |
| ---             colliding is vanishingly unlikely, which is how GUIDs work                  --- |
-------------------------------------------------------------------------------------------------*/
std::uint64_t CaledonEngine::GameObject::GenerateID()
{
	static std::mt19937_64 generator{ std::random_device{}() ^ static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()) };

	std::uint64_t id = 0;
	while (id == 0)
	{
		id = generator();
	}
	return id;
}

/*---------------------------------------------------------------------------------------------------------
| --- AttachChild: Takes ownership of a child (no initialization; used by AddChild, SetParent, Scene) --- |
---------------------------------------------------------------------------------------------------------*/
void CaledonEngine::GameObject::AttachChild(std::unique_ptr<GameObject> pChild)
{
	if (!pChild)
		return;

	pChild->m_pParent = this;
	pChild->SetScene(m_pScene);
	pChild->GetTransform().MarkWorldDirty();
	m_children.emplace_back(std::move(pChild));
}

/*--------------------------------------------------------------------------------------
| --- ReleaseChild: Hands ownership of a child to the caller without destroying it --- |
--------------------------------------------------------------------------------------*/
std::unique_ptr<CaledonEngine::GameObject> CaledonEngine::GameObject::ReleaseChild(GameObject* pChild)
{
	auto it = std::find_if(m_children.begin(), m_children.end(),
		[pChild](const std::unique_ptr<GameObject>& p) { return p.get() == pChild; });

	if (it == m_children.end())
		return nullptr;

	std::unique_ptr<GameObject> pReleased = std::move(*it);
	m_children.erase(it);

	pReleased->m_pParent = nullptr;
	pReleased->GetTransform().MarkWorldDirty();
	return pReleased;
}

/*----------------------------------------------------------------------------------------------
| --- ReleaseComponent: Hands ownership of a Component to the caller without destroying it --- |
----------------------------------------------------------------------------------------------*/
std::unique_ptr<CaledonEngine::Component> CaledonEngine::GameObject::ReleaseComponent(Component* pComponent)
{
	if (pComponent == nullptr || pComponent == m_pTransform)
		return nullptr;

	auto it = std::find_if(m_components.begin(), m_components.end(),
		[pComponent](const std::unique_ptr<Component>& p) { return p.get() == pComponent; });

	if (it == m_components.end())
		return nullptr;

	std::unique_ptr<Component> pReleased = std::move(*it);
	m_components.erase(it);
	return pReleased;
}

/*---------------------------------------------------------------------------------------------------------
| --- DestroyImmediate: Destroys the children and Components right away. Only the destructor and the  --- |
| ---                   Scene's end-of-frame flush use it; gameplay code calls Destroy() instead      --- |
| ---                   Each list is emptied back to front with pop_back, because std::vector doesn't --- |
| ---                   promise an order when it is cleared. The Transform is element 0, so it goes   --- |
| ---                   last and the other Components can still read it while they are destroyed      --- |
---------------------------------------------------------------------------------------------------------*/
void CaledonEngine::GameObject::DestroyImmediate() noexcept
{
	while (!m_children.empty())
	{
		m_children.pop_back();
	}

	while (!m_components.empty())
	{
		m_components.pop_back();
	}

	m_pTransform = nullptr;
	m_pParent = nullptr;
}
