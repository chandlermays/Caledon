/*------------------------------
| File: EditorSubsystem.cpp
| Author: Chandler Mays
------------------------------*/
#include "EditorSubsystem.h"

#include "CaledonEngine/Systems/Engine/EngineContext.h"
#include "CaledonEngine/Systems/Engine/LoggingSubsystem.h"
#include "CaledonEngine/Systems/Input/Input.h"
#include "CaledonEngine/Systems/Input/InputManager.h"
#include "CaledonEngine/Systems/Rendering/Renderer.h"
#include "CaledonEngine/Systems/Rendering/RenderingSubsystem.h"
#include "CaledonEngine/Systems/Window/Window.h"
#include "CaledonEngine/Systems/Window/WindowSubsystem.h"

#include <ImGUI/imgui.h>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------
| --- Constructor: Constructs the EditorSubsystem with default values --- |
-------------------------------------------------------------------------*/
CaledonEditor::EditorSubsystem::EditorSubsystem()
	: m_pContext{ nullptr }
	, m_pInput{ nullptr }
	, m_gui{}
	, m_showImGuiDemo{ false }
{
}

/*------------------------------------------------------------
| --- Destructor: Shuts the Editor down if still running --- |
------------------------------------------------------------*/
CaledonEditor::EditorSubsystem::~EditorSubsystem()
{
	Shutdown();
}

/*--------------------------------------------
| --- Name: Returns the Subsystem's name --- |
--------------------------------------------*/
std::string_view CaledonEditor::EditorSubsystem::Name() const noexcept
{
	return kName;
}

/*----------------------------------------------------------------------
| --- Initialize: Starts ImGui on the engine's window and renderer --- |
----------------------------------------------------------------------*/
bool CaledonEditor::EditorSubsystem::Initialize(CaledonEngine::EngineContext& context)
{
	using namespace CaledonEngine;

	WindowSubsystem* pWindowSubsystem = context.Get<WindowSubsystem>();
	RenderingSubsystem* pRendering = context.Get<RenderingSubsystem>();
	InputSubsystem* pInput = context.Get<InputSubsystem>();

	Window* pWindow = pWindowSubsystem ? pWindowSubsystem->GetWindow() : nullptr;
	Renderer* pRenderer = pRendering ? pRendering->GetRenderer() : nullptr;
	Input* pInputAPI = pInput ? pInput->GetInputAPI() : nullptr;
	if (!pWindow || !pRenderer || !pInputAPI)
	{
		CE_LOG(context, LogLevel::Error, "Editor", "Needs the Window, Input and Rendering subsystems. Register it after them");
		return false;
	}

	if (!m_gui.Initialize(pWindow->GetNativeHandle(), pRenderer->GetNativeHandle()))
	{
		CE_LOG(context, LogLevel::Error, "Editor", "ImGui failed to start on the SDL3 window and renderer");
		return false;
	}

	// Every SDL event the InputSubsystem polls is shown to ImGui first
	pInputAPI->SetEventCallback([this](const void* pEvent)
		{
			m_gui.ProcessEvent(pEvent);
		});

	m_pContext = &context;
	m_pInput = pInput;

	CE_LOG(context, LogLevel::Display, "Editor", "Editor ready (ImGui {})", ImGui::GetVersion());
	return true;
}

/*----------------------------------------------------------------
| --- Shutdown: Stops forwarding events and shuts ImGui down --- |
----------------------------------------------------------------*/
void CaledonEditor::EditorSubsystem::Shutdown() noexcept
{
	// The InputSubsystem outlives the Editor (it was registered first), so the callback must not point at a dead Editor
	if (m_pInput && m_pInput->GetInputAPI())
	{
		m_pInput->GetInputAPI()->SetEventCallback(nullptr);
	}

	m_gui.Shutdown();

	m_pInput = nullptr;
	m_pContext = nullptr;
}

/*-----------------------------------------------------
| --- BeginFrame: Starts the Editor's ImGui frame --- |
-----------------------------------------------------*/
void CaledonEditor::EditorSubsystem::BeginFrame()
{
	m_gui.BeginFrame();
}

/*---------------------------------------------------------------------------
| --- Render: Draws the Editor's windows and hands them to the renderer --- |
---------------------------------------------------------------------------*/
void CaledonEditor::EditorSubsystem::Render()
{
	if (!m_gui.IsInitialized())
		return;

	DrawMainMenuBar();
	m_gui.DrawDockspace();

	if (m_showImGuiDemo)
	{
		ImGui::ShowDemoWindow(&m_showImGuiDemo);
	}

	m_gui.EndFrame();
}

/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*------------------------------------------------------------------------------------
| --- DrawMainMenuBar: Draws the File and View menus along the top of the window --- |
------------------------------------------------------------------------------------*/
void CaledonEditor::EditorSubsystem::DrawMainMenuBar()
{
	if (!ImGui::BeginMainMenuBar())
		return;

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Exit", "Alt+F4") && m_pContext)
		{
			m_pContext->RequestQuit();
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("ImGui Demo", nullptr, &m_showImGuiDemo);
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}
