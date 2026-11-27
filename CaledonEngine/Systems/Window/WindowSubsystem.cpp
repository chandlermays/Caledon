/*------------------------------
| File: WindowSubsystem.cpp
| Author: Chandler Mays
------------------------------*/
#include "WindowSubsystem.h"

#include "Window.h"
#include "API/SDL/SDLWindow.h"
#include "Systems/Engine/EngineContext.h"
#include "Systems/Engine/LoggingSubsystem.h"

#include <format>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------
| --- Constructor: Constructs the WindowSubsystem with no window yet --- |
------------------------------------------------------------------------*/
CaledonEngine::WindowSubsystem::WindowSubsystem(const WindowConfig& config)
	: m_config{ config }
	, m_pWindowAPI{ nullptr }
{
}

/*---------------------------------------------------------------------------
| --- Destructor: Defined here so Window can stay a forward declaration --- |
---------------------------------------------------------------------------*/
CaledonEngine::WindowSubsystem::~WindowSubsystem() = default;

/*--------------------------------------------
| --- Name: Returns the Subsystem's name --- |
--------------------------------------------*/
std::string_view CaledonEngine::WindowSubsystem::Name() const noexcept
{
	return kName;
}

/*-------------------------------------------------------------------
| --- Initialize: Opens the window through the platform backend --- |
-------------------------------------------------------------------*/
bool CaledonEngine::WindowSubsystem::Initialize([[maybe_unused]] EngineContext& context)
{
	// Only keep the window once it has fully opened, so a failure leaves nothing behind.
	std::unique_ptr<Window> pWindowAPI = std::make_unique<SDLWindow>(m_config);
	if (!pWindowAPI->Initialize())
	{
		CE_LOG(context, LogLevel::Error, "Window", std::format("Window failed to open: {}", pWindowAPI->GetLastError()));
		return false;
	}

	m_pWindowAPI = std::move(pWindowAPI);

	CE_LOG(context, LogLevel::Display, "Window", std::format("Opened window '{}' ({}x{})",
		m_pWindowAPI->GetTitle(), m_pWindowAPI->GetWidth(), m_pWindowAPI->GetHeight()));
	return true;
}

/*--------------------------------------------------
| --- Shutdown: Closes and destroys the window --- |
--------------------------------------------------*/
void CaledonEngine::WindowSubsystem::Shutdown() noexcept
{
	if (!m_pWindowAPI)
		return;

	m_pWindowAPI->Shutdown();
	m_pWindowAPI.reset();
}

/*--------------------------------------------------------------------
| --- GetWindow: Returns the window, or nullptr if it isn't open --- |
--------------------------------------------------------------------*/
CaledonEngine::Window* CaledonEngine::WindowSubsystem::GetWindow() const noexcept
{
	return m_pWindowAPI.get();
}
