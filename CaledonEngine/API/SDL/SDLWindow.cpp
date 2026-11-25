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
{
}

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
	if (m_pWindow)
		return true;

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		m_lastError = SDL_GetError();
		return false;
	}

	SDL_WindowFlags windowFlags = 0;
	if (m_isFullscreen)
	{
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}
	if (m_isResizable)
	{
		windowFlags |= SDL_WINDOW_RESIZABLE;
	}

	m_pWindow = SDL_CreateWindow(m_title.c_str(), m_width, m_height, windowFlags);
	if (!m_pWindow)
	{
		m_lastError = SDL_GetError();
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		return false;
	}

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

	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

/*------------------------------------------------
| --- SetTitle: Sets the title of the window --- |
------------------------------------------------*/
void CaledonEngine::SDLWindow::SetTitle(const std::string& title)
{
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
	m_isFullscreen = isFullscreen;

	if (m_pWindow)
	{
		SDL_SetWindowFullscreen(m_pWindow, m_isFullscreen);
	}
}

/*--------------------------------
| --- Show: Shows the window --- |
--------------------------------*/
void CaledonEngine::SDLWindow::Show()
{
	if (m_pWindow)
	{
		m_isMinimized = false;
		m_isFocused = true;
		SDL_ShowWindow(m_pWindow);
	}
}

/*--------------------------------
| --- Hide: Hides the window --- |
--------------------------------*/
void CaledonEngine::SDLWindow::Hide()
{
	if (m_pWindow)
	{
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

/*-----------------------------------------------------------------------------------------
| --- GetWidth: Returns the current width of the window. It is read from SDL, because --- |
| ---           the user can resize the window and the cached size would go stale     --- |
-----------------------------------------------------------------------------------------*/
int CaledonEngine::SDLWindow::GetWidth() const
{
	if (!m_pWindow)
		return m_width;

	int width = 0;
	SDL_GetWindowSize(m_pWindow, &width, nullptr);
	return width;
}

/*-------------------------------------------------------------------------------------------
| --- GetHeight: Returns the current height of the window. It is read from SDL, because --- |
| ---            the user can resize the window and the cached size would go stale      --- |
-------------------------------------------------------------------------------------------*/
int CaledonEngine::SDLWindow::GetHeight() const
{
	if (!m_pWindow)
		return m_height;

	int height = 0;
	SDL_GetWindowSize(m_pWindow, nullptr, &height);
	return height;
}
