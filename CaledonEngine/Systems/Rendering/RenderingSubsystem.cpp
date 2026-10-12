/*------------------------------
| File: RenderingSubsystem.cpp
| Author: Chandler Mays
------------------------------*/
#include "RenderingSubsystem.h"

#include "Renderer.h"
#include "API/SDL/SDLRenderer.h"
#include "Systems/Engine/EngineContext.h"
#include "Systems/Engine/LoggingSubsystem.h"
#include "Systems/Window/WindowSubsystem.h"

#include <format>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------------
| --- Constructor: Constructs the RenderingSubsystem with default values --- |
----------------------------------------------------------------------------*/
CaledonEngine::RenderingSubsystem::RenderingSubsystem()
	: m_pRendererAPI{ nullptr }
{ }

/*-----------------------------------------------------------------------------
| --- Destructor: Defined here so Renderer can stay a forward declaration --- |
-----------------------------------------------------------------------------*/
CaledonEngine::RenderingSubsystem::~RenderingSubsystem() = default;

/*---------------------------------------------------------------------------------------
| --- Name:
---------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::RenderingSubsystem::Name() const noexcept
{
	return kName;
}

/*---------------------------------------------------------------------------------------
| --- Initialize:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::RenderingSubsystem::Initialize(EngineContext& context)
{
	WindowSubsystem* pWindowSubsystem = context.Get<WindowSubsystem>();
	if (!pWindowSubsystem || !pWindowSubsystem->GetWindow())
	{
		CE_LOG(context, LogLevel::Error, "Rendering", "No window is open. Register the WindowSubsystem before the RenderingSubsystem");
		return false;
	}

	// Only keep the renderer once it has fully started, so a failure leaves nothing behind.
	std::unique_ptr<Renderer> pRendererAPI = std::make_unique<SDLRenderer>();
	if (!pRendererAPI->Initialize(pWindowSubsystem->GetWindow()))
	{
		CE_LOG(context, LogLevel::Error, "Rendering", std::format("Renderer failed to start: {}", pRendererAPI->GetLastError()));
		return false;
	}

	m_pRendererAPI = std::move(pRendererAPI);

	CE_LOG(context, LogLevel::Display, "Rendering", "Renderer started (SDL)");
	return true;
}

/*---------------------------------------------------------------------------------------
| --- Shutdown:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::Shutdown() noexcept
{
	if (!m_pRendererAPI)
		return;

	m_pRendererAPI->Shutdown();
	m_pRendererAPI.reset();
}

/*---------------------------------------------------------------------------------------
| --- Render:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::Render()
{
	BeginFrame();
	EndFrame();
}

/*---------------------------------------------------------------------------------------
| --- BeginFrame:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::BeginFrame()
{
	if (m_pRendererAPI)
	{
		m_pRendererAPI->BeginFrame();
	}
}

/*---------------------------------------------------------------------------------------
| --- EndFrame:
---------------------------------------------------------------------------------------*/
void CaledonEngine::RenderingSubsystem::EndFrame()
{
	if (m_pRendererAPI)
	{
		m_pRendererAPI->EndFrame();
	}
}

/*---------------------------------------------------------------------------------------
| --- GetRenderer:
---------------------------------------------------------------------------------------*/
CaledonEngine::Renderer* CaledonEngine::RenderingSubsystem::GetRenderer() const noexcept
{
	return m_pRendererAPI.get();
}