/*------------------------------
| File: Scene.cpp
| Author: Chandler Mays
------------------------------*/
#include "Scene.h"

#include "Systems/Scene/GameObject.h"

#include <algorithm>

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
{ }

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
	bool success = true;
	for (const auto& pObject : m_gameObjects)
	{
		if (pObject != nullptr && !pObject->Initialize())
		{
			success = false;
		}
	}
	m_isLoaded = true;
	return success;
}

/*--------------------------------------------------------
| --- Update: Updates this Scene and its GameObjects --- |
--------------------------------------------------------*/
void CaledonEngine::Scene::Update(float deltaTime)
{
	for (const auto& pObject : m_gameObjects)
	{
		if (pObject->IsActive())
		{
			pObject->Update(deltaTime);
		}
	}
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

/*---------------------------------------------------------------
| --- IsDirty: 
---------------------------------------------------------------*/
bool CaledonEngine::Scene::IsDirty() const
{
	return m_isDirty;
}

/*---------------------------------------------------------------
| --- SetDirty:
---------------------------------------------------------------*/
void CaledonEngine::Scene::SetDirty(bool dirty)
{
	m_isDirty = dirty;
}

/*---------------------------------------------------------------
| --- MarkClean:
---------------------------------------------------------------*/
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

/*---------------------------------------------------------------
| --- GetFilePath: 
---------------------------------------------------------------*/
const std::string& CaledonEngine::Scene::GetFilePath() const
{
	return m_filePath;
}

/*---------------------------------------------------------------
| --- SetFilePath:
---------------------------------------------------------------*/
void CaledonEngine::Scene::SetFilePath(const std::string& filePath)
{
	m_filePath = filePath;
}

/*--------------------------------------------------------
| --- AddGameObject: Adds a GameObject to this Scene --- |
--------------------------------------------------------*/
void CaledonEngine::Scene::AddGameObject(std::unique_ptr<GameObject> pGameObject)
{
	if (pGameObject)
	{
		pGameObject->SetScene(this);
		m_gameObjects.emplace_back(std::move(pGameObject));
	}
}

/*----------------------------------------------------------------
| --- RemoveGameObject: Removes a GameObject from this Scene --- |
----------------------------------------------------------------*/
void CaledonEngine::Scene::RemoveGameObject(GameObject* pGameObject)
{
	auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(),
		[pGameObject](const std::unique_ptr<GameObject>& p)
		{
			return p.get() == pGameObject;
		});

	if (it != m_gameObjects.end())
		m_gameObjects.erase(it);
}

/*----------------------------------------------------------------------------------------------
| --- GetGameObjects: Returns a vector of unique pointers to the GameObjects in this Scene --- |
----------------------------------------------------------------------------------------------*/
const std::vector<std::unique_ptr<CaledonEngine::GameObject>>& CaledonEngine::Scene::GetGameObjects() const
{
	return m_gameObjects;
}

/*-------------------------------------------------------
| --- Clear: Clears all GameObjects from this Scene --- |
-------------------------------------------------------*/
void CaledonEngine::Scene::Clear()
{
	m_gameObjects.clear();
}