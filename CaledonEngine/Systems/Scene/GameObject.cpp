/*------------------------------
| File: GameObject.cpp
| Author: Chandler Mays
------------------------------*/
#include "GameObject.h"

#include "Systems/Scene/Component.h"
#include "Systems/Scene/Transform.h"

#include <algorithm>
#include <cassert>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*--------------------------------------------------------------------
| --- Constructor: Constructs the GameObject with default values --- |
--------------------------------------------------------------------*/
CaledonEngine::GameObject::GameObject()
	: m_pScene { nullptr }
	, m_pParent{ nullptr }
	, m_name{ "GameObject" }
	, m_tag{ "Untagged" }
	, m_layer{ 0 }
	, m_isActive{ true }
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
	Destroy();
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

/*------------------------------------------------------------
| --- Update: Updates this GameObject and its components --- |
------------------------------------------------------------*/
void CaledonEngine::GameObject::Update(float deltaTime)
{
	if (!m_isActive)
		return;

	for (const auto& pComponent : m_components)
	{
		if (pComponent != nullptr && pComponent->IsActive())
		{
			pComponent->Update(deltaTime);
		}
	}

	for (const auto& pChild : m_children)
	{
		if (pChild != nullptr && pChild->IsActive())
		{
			pChild->Update(deltaTime);
		}
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

/*-------------------------------------------------------
| --- SetParent: Sets the parent of this GameObject --- |
-------------------------------------------------------*/
void CaledonEngine::GameObject::SetParent(GameObject* pParent)
{
	m_pParent = pParent;
}

/*---------------------------------------------------------------------------------
| --- GetChildren: Returns a vector of pointers to this GameObject's children --- |
---------------------------------------------------------------------------------*/
const std::vector<std::unique_ptr<CaledonEngine::GameObject>>& CaledonEngine::GameObject::GetChildren() const
{
	return m_children;
}

/*---------------------------------------------------
| --- AddChild: Adds a child to this GameObject --- |
---------------------------------------------------*/
void CaledonEngine::GameObject::AddChild(std::unique_ptr<GameObject> pChild)
{
	if (pChild != nullptr)
	{
		pChild->SetParent(this);
		pChild->SetScene(m_pScene);
		m_children.emplace_back(std::move(pChild));
	}
}

/*-----------------------------------------------------------
| --- RemoveChild: Removes a child from this GameObject --- |
-----------------------------------------------------------*/
void CaledonEngine::GameObject::RemoveChild(GameObject* pChild)
{
	auto it = std::find_if(m_children.begin(), m_children.end(),
		[pChild](const std::unique_ptr<GameObject>& p) { return p.get() == pChild; });

	if (it != m_children.end())
	{
		(*it)->SetParent(nullptr);
		m_children.erase(it);
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

/*-----------------------------------------------------------
| --- AddComponent: Adds a component to this GameObject --- |
-----------------------------------------------------------*/
void CaledonEngine::GameObject::AddComponent(std::unique_ptr<Component> pComponent)
{
	if (!pComponent)
		return;

	pComponent->SetOwner(this);
	m_components.emplace_back(std::move(pComponent));
}

/*-------------------------------------------------------------------
| --- RemoveComponent: Removes a component from this GameObject --- |
-------------------------------------------------------------------*/
void CaledonEngine::GameObject::RemoveComponent(Component* pComponent)
{
	if (pComponent == nullptr || pComponent == m_pTransform)
		return;

	auto it = std::find_if(m_components.begin(), m_components.end(),
		[pComponent](const std::unique_ptr<Component>& p) { return p.get() == pComponent; });

	if (it != m_components.end())
	{
		(*it)->SetOwner(nullptr);
		m_components.erase(it);
	}
}

/*--------------------------------------------------------------
| --- Destroy: Destroys this GameObject and its components --- |
--------------------------------------------------------------*/
void CaledonEngine::GameObject::Destroy()
{
	m_children.clear();
	m_components.clear();
	m_pTransform = nullptr;
	m_pParent = nullptr;
}