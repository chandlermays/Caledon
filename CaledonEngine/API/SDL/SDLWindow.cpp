/*------------------------------
| File: SDLWindow.cpp
| Author: Chandler Mays
------------------------------*/
#include "SDLWindow.h"

#include <SDL3/SDL.h>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------
| --- Constructor: Constructs the 'SDL' Window with default values --- |
----------------------------------------------------------------------*/
CaledonEngine::SDLWindow::SDLWindow(const WindowConfig& config)
	: Window{ config }
	, m_pWindow{ nullptr }
{ }

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::SDLWindow::~SDLWindow()
{
	Shutdown();
}

/*-----------------------------------------------------
| --- Initialize: Prepares the SDL Window for use --- |
-----------------------------------------------------*/
bool CaledonEngine::SDLWindow::Initialize()
{
	// Keep initialization idempotent when the native window already exists.
	if (m_pWindow)
		return true;

	// SDL video must be initialized before creating a native window.
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		m_lastError = SDL_GetError();
		return false;
	}

	// Translate the engine's window options into SDL creation flags.
	SDL_WindowFlags windowFlags = 0;
	if (m_isFullscreen)
	{
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}
	if (m_isResizable)
	{
		windowFlags |= SDL_WINDOW_RESIZABLE;
	}

	// Release the video subsystem if native window creation fails.
	m_pWindow = SDL_CreateWindow(m_title.c_str(), m_width, m_height, windowFlags);
	if (!m_pWindow)
	{
		m_lastError = SDL_GetError();
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		return false;
	}

	// Place newly created windows in the center of the display.
	SDL_SetWindowPosition(m_pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	m_lastError.clear();
	return true;
}

/*-------------------------------------------------------------
| --- Shutdown: Cleans up and shuts down the 'SDL' Window --- |
-------------------------------------------------------------*/
void CaledonEngine::SDLWindow::Shutdown() noexcept
{
	if (!m_pWindow)
		return;

	// Destroy the window before shutting down the subsystem it depends on.
	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

/*------------------------------------------------
| --- SetTitle: Sets the title of the window --- |
------------------------------------------------*/
void CaledonEngine::SDLWindow::SetTitle(const std::string& title)
{
	// Cache settings even before initialization so they take effect at creation.
	m_title = title;

	if (m_pWindow)
	{
		SDL_SetWindowTitle(m_pWindow, m_title.c_str());
	}
}

/*----------------------------------------------
| --- SetSize: Sets the size of the window --- |
----------------------------------------------*/
void CaledonEngine::SDLWindow::SetSize(int width, int height)
{
	// Keep the requested size for initialization and update a live window if present.
	m_width = width;
	m_height = height;

	if (m_pWindow)
	{
		SDL_SetWindowSize(m_pWindow, m_width, m_height);
	}
}

/*--------------------------------------------------
| --- SetFullscreen: Sets the fullscreen state --- |
--------------------------------------------------*/
void CaledonEngine::SDLWindow::SetFullscreen(bool isFullscreen)
{
	// Preserve the requested mode for initialization and apply it immediately if live.
	m_isFullscreen = isFullscreen;

	if (m_pWindow)
	{
		SDL_SetWindowFullscreen(m_pWindow, m_isFullscreen);
	}
}

/*-------------------------------------------------------------
| --- Show: Shows the window and updates its cached state --- |
-------------------------------------------------------------*/
void CaledonEngine::SDLWindow::Show()
{
	if (m_pWindow)
	{
		// Update the engine's cached window state alongside the SDL visibility change.
		m_isMinimized = false;
		m_isFocused = true;
		SDL_ShowWindow(m_pWindow);
	}
}

/*-------------------------------------------------------------
| --- Hide: Hides the window and updates its cached state --- |
-------------------------------------------------------------*/
void CaledonEngine::SDLWindow::Hide()
{
	if (m_pWindow)
	{
		// Update the engine's cached window state alongside the SDL visibility change.
		m_isMinimized = true;
		m_isFocused = false;
		SDL_HideWindow(m_pWindow);
	}
}

/*-----------------------------------------------------------
| --- GetNativeHandle: Returns the native window handle --- |
-----------------------------------------------------------*/
void* CaledonEngine::SDLWindow::GetNativeHandle() const
{
	return static_cast<void*>(m_pWindow);
}

/*---------------------------------------------------------------------------------------------
| --- GetWidth: Returns the current window width so user resizing is reflected accurately --- |
---------------------------------------------------------------------------------------------*/
int CaledonEngine::SDLWindow::GetWidth() const
{
	// Use the configured size before creation; afterward SDL reflects user resizing.
	if (!m_pWindow)
		return m_width;

	int width = 0;
	SDL_GetWindowSize(m_pWindow, &width, nullptr);
	return width;
}

/*-----------------------------------------------------------------------------------------------
| --- GetHeight: Returns the current window height so user resizing is reflected accurately --- |
-----------------------------------------------------------------------------------------------*/
int CaledonEngine::SDLWindow::GetHeight() const
{
	// Use the configured size before creation; afterward SDL reflects user resizing.
	if (!m_pWindow)
		return m_height;

	int height = 0;
	SDL_GetWindowSize(m_pWindow, nullptr, &height);
	return height;
}