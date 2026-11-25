/*------------------------------
| File: EditorGUI.cpp
| Author: Chandler Mays
------------------------------*/
#include "EditorGUI.h"

#include <SDL3/SDL.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_sdl3.h>
#include <ImGUI/imgui_impl_sdlrenderer3.h>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------
| --- Constructor: Constructs the EditorGUI with default values --- |
-------------------------------------------------------------------*/
CaledonEditor::EditorGUI::EditorGUI()
	: m_pNativeRenderer{ nullptr }
	, m_isInitialized{ false }
{
}

/*---------------------------------------------------------------------------
| --- Destructor: Shuts ImGui down if Shutdown() was not already called --- |
---------------------------------------------------------------------------*/
CaledonEditor::EditorGUI::~EditorGUI()
{
	Shutdown();
}

/*----------------------------------------------------------------------------
| --- Initialize: Creates the ImGui context and starts the SDL3 backends --- |
----------------------------------------------------------------------------*/
bool CaledonEditor::EditorGUI::Initialize(void* pNativeWindow, void* pNativeRenderer)
{
	if (m_isInitialized)
		return true;

	SDL_Window* pWindow = static_cast<SDL_Window*>(pNativeWindow);
	SDL_Renderer* pRenderer = static_cast<SDL_Renderer*>(pNativeRenderer);
	if (!pWindow || !pRenderer)
		return false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;						// Multi-viewports (panels dragged outside the window) are not supported by the SDL_Renderer backend

	ImGui::StyleColorsDark();

	if (!ImGui_ImplSDL3_InitForSDLRenderer(pWindow, pRenderer))
	{
		ImGui::DestroyContext();
		return false;
	}

	if (!ImGui_ImplSDLRenderer3_Init(pRenderer))
	{
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
		return false;
	}

	m_pNativeRenderer = pNativeRenderer;
	m_isInitialized = true;
	return true;
}

/*---------------------------------------------------------------------
| --- Shutdown: Stops the backends and destroys the ImGui context --- |
---------------------------------------------------------------------*/
void CaledonEditor::EditorGUI::Shutdown() noexcept
{
	if (!m_isInitialized)
		return;

	// The SDL3 backend stores the window's ID in the main viewport, and this ImGui version asserts on shutdown
	// ("Backend or app forgot to call DestroyPlatformWindows()?") unless every viewport is cleared first
	ImGui::DestroyPlatformWindows();

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	m_pNativeRenderer = nullptr;
	m_isInitialized = false;
}

/*----------------------------------------------------
| --- ProcessEvent: Passes an SDL_Event to ImGui --- |
----------------------------------------------------*/
void CaledonEditor::EditorGUI::ProcessEvent(const void* pEvent)
{
	if (!m_isInitialized || !pEvent)
		return;

	ImGui_ImplSDL3_ProcessEvent(static_cast<const SDL_Event*>(pEvent));
}

/*----------------------------------------------
| --- BeginFrame: Starts a new ImGui frame --- |
----------------------------------------------*/
void CaledonEditor::EditorGUI::BeginFrame()
{
	if (!m_isInitialized)
		return;

	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

/*-------------------------------------------------------------------------
| --- EndFrame: Finishes the ImGui frame and draws it onto the window --- |
-------------------------------------------------------------------------*/
void CaledonEditor::EditorGUI::EndFrame()
{
	if (!m_isInitialized)
		return;

	ImGui::Render();

	// The RenderingSubsystem has already drawn the scene this frame and presents in its EndFrame, so ImGui draws on top of it
	SDL_Renderer* pRenderer = static_cast<SDL_Renderer*>(m_pNativeRenderer);
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), pRenderer);
}

/*------------------------------------------------------------------------------------
| --- DrawDockspace: Makes the whole window a dockspace the panels can dock into --- |
------------------------------------------------------------------------------------*/
void CaledonEditor::EditorGUI::DrawDockspace()
{
	if (!m_isInitialized)
		return;

	// PassthruCentralNode leaves the empty middle see-through, so the scene shows until the Viewport panel exists (Stage 5g)
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

/*--------------------------------------------------------------------------------------------
| --- WantsKeyboard: Checks if ImGui is using the keyboard (e.g. a text field has focus) --- |
--------------------------------------------------------------------------------------------*/
bool CaledonEditor::EditorGUI::WantsKeyboard() const
{
	return m_isInitialized && ImGui::GetIO().WantCaptureKeyboard;
}

/*----------------------------------------------------------------------------------
| --- WantsMouse: Checks if ImGui is using the mouse (e.g. it is over a panel) --- |
----------------------------------------------------------------------------------*/
bool CaledonEditor::EditorGUI::WantsMouse() const
{
	return m_isInitialized && ImGui::GetIO().WantCaptureMouse;
}

/*-------------------------------------------------------------------
| --- IsInitialized: Checks if ImGui and its backends are ready --- |
-------------------------------------------------------------------*/
bool CaledonEditor::EditorGUI::IsInitialized() const noexcept
{
	return m_isInitialized;
}