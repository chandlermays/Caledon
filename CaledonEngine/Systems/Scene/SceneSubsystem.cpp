#include "SceneSubsystem.h"

#include "Systems/Scene/Scene.h"
#include "Systems/Engine/EngineContext.h"
#include "Systems/Engine/EngineManager.h"
#include "Systems/Engine/LoggingSubsystem.h"
#include "Systems/Rendering/RenderingSubsystem.h"
#include "Systems/Rendering/Renderer.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------
| --- Constructor: Constructs the SceneSubsystem with default values --- |
------------------------------------------------------------------------*/
CaledonEngine::SceneSubsystem::SceneSubsystem()
	: m_pCurrentScene{ nullptr }
	, m_pRenderer{ nullptr }
	, m_pRenderTarget{ nullptr }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::SceneSubsystem::~SceneSubsystem()
{
	Shutdown();
}

/*------------------------------------------------------------------------
| --- Initialize: 
------------------------------------------------------------------------*/
bool CaledonEngine::SceneSubsystem::Initialize(EngineContext& context)
{
	return false;
}

/*------------------------------------------------------------------------
| --- Shutdown:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::Shutdown() noexcept
{
	UnloadAllScenes();
	m_pRenderer = nullptr;
	m_pRenderTarget = nullptr;
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
| --- Render:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::Render()
{
	if (m_pCurrentScene == nullptr || !m_pCurrentScene->IsActive())
		return;

	if (m_pRenderer && m_pRenderTarget)
	{
		m_pRenderer->SetRenderTarget(m_pRenderTarget);
		m_pRenderer->Clear();
	}

	m_pCurrentScene->Render();

	if (m_pRenderer && m_pRenderTarget)
	{
		m_pRenderer->SetRenderTarget(nullptr);
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
void CaledonEngine::SceneSubsystem::AddScene(std::unique_ptr<Scene> pScene)
{
	if (!pScene)
		return;

	if (m_pCurrentScene == nullptr)
	{
		m_pCurrentScene = pScene.get();
	}

	m_pScenes.emplace_back(std::move(pScene));
}

/*------------------------------------------------------------------------
| --- RemoveScene:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::RemoveScene(std::unique_ptr<Scene> pScene)
{
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

/*------------------------------------------------------------------------
| --- GetRenderTarget:
------------------------------------------------------------------------*/
CaledonEngine::Texture* CaledonEngine::SceneSubsystem::GetRenderTarget() const
{
	return m_pRenderTarget;
}

/*------------------------------------------------------------------------
| --- SetRenderTarget:
------------------------------------------------------------------------*/
void CaledonEngine::SceneSubsystem::SetRenderTarget(Texture* pTexture)
{
	m_pRenderTarget = pTarget;
}