#include "SceneSubsystem.h"

#include "Systems/Scene/Scene.h"
#include "Systems/Engine/EngineContext.h"
#include "Systems/Engine/LoggingSubsystem.h"
#include "Systems/Physics/Physics2DSubsystem.h"

#include <algorithm>
#include <format>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------
| --- Constructor: Constructs the SceneSubsystem with default values --- |
------------------------------------------------------------------------*/
CaledonEngine::SceneSubsystem::SceneSubsystem()
	: m_pCurrentScene{ nullptr }
	, m_pContext{ nullptr }
	, m_pPhysics2D{ nullptr }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::SceneSubsystem::~SceneSubsystem()
{
	Shutdown();
}

/*------------------------------------------------------------------------
| --- Name:
------------------------------------------------------------------------*/
std::string_view CaledonEngine::SceneSubsystem::Name() const noexcept
{
	return kName;
}

/*------------------------------------------------------------------------
| --- Initialize: 
------------------------------------------------------------------------*/
bool CaledonEngine::SceneSubsystem::Initialize([[maybe_unused]] EngineContext& context)
{
	m_pContext = &context;
	m_pPhysics2D = context.Get<Physics2DSubsystem>();

	for (const auto& pScene : m_pScenes)
	{
		if (!InitializeScene(*pScene))
			return false;
	}
	return true;
}

/*------------------------------------------------------------------------
| --- Shutdown:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::Shutdown() noexcept
{
	UnloadAllScenes();
	m_pContext = nullptr;
	m_pPhysics2D = nullptr;
}

/*------------------------------------------------------------------------
| --- Update:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::Update(float deltaTime)
{
	if (m_pCurrentScene != nullptr && m_pCurrentScene->IsActive())
	{
		m_pCurrentScene->Update(deltaTime);
	}
}

/*------------------------------------------------------------------------
| --- UnloadAllScenes:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::UnloadAllScenes()
{
	m_pScenes.clear();
	m_pCurrentScene = nullptr;
}

/*------------------------------------------------------------------------
| --- AddScene:
------------------------------------------------------------------------*/
bool CaledonEngine::SceneSubsystem::AddScene(std::unique_ptr<Scene> pScene)
{
	if (!pScene)
		return;

	if (m_pContext != nullptr && !InitializeScene(*pScene))
		return false;

	if (m_pCurrentScene == nullptr)
	{
		m_pCurrentScene = pScene.get();
	}
		
	m_pScenes.emplace_back(std::move(pScene));
	return true;
}

/*------------------------------------------------------------------------
| --- RemoveScene:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::RemoveScene(Scene* pScene)
{
	if (pScene == nullptr)
		return;

	if (m_pCurrentScene == pScene)
	{
		m_pCurrentScene = nullptr;
	}

	m_pScenes.erase(std::remove_if(m_pScenes.begin(), m_pScenes.end(),
		[pScene](const std::unique_ptr<Scene>& p) { return p.get() == pScene; }), m_pScenes.end());
}

/*------------------------------------------------------------------------
| --- GetCurrentScene:
------------------------------------------------------------------------*/
CaledonEngine::Scene* CaledonEngine::SceneSubsystem::GetCurrentScene() const
{
	return m_pCurrentScene;
}

/*------------------------------------------------------------------------
| --- SetCurrentScene:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::SetCurrentScene(Scene* pScene)
{
	if (pScene != nullptr)
	{
		m_pCurrentScene = pScene;
	}
}

/*------------------------------------------------------------------------
| --- GetSceneIndex:
------------------------------------------------------------------------*/
int CaledonEngine::SceneSubsystem::GetSceneIndex(Scene* pScene) const
{
	if (!pScene)
		return -1;

	for (int i = 0; i < static_cast<int>(m_pScenes.size()); ++i)
	{
		if (m_pScenes[i].get() == pScene)
			return i;
	}

	return -1;
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*------------------------------------------------------------------------
| --- InitializeScene:
------------------------------------------------------------------------*/
bool CaledonEngine::SceneSubsystem::InitializeScene(Scene& scene)
{
	scene.SetPhysicsScene2D(m_pPhysics2D ? &m_pPhysics2D->GetDefaultPhysicsScene() : nullptr);

	if (!scene.Initialize())
	{
		CE_LOG(*m_pContext, LogLevel::Error, "Scene", std::format("Scene '{}' failed to initialize", scene.GetName()));
		return false;
	}
	return true;
}