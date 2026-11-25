/*------------------------------
| File: Window.cpp
| Author: Chandler Mays
------------------------------*/
#include "Window.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------
| --- Constructor: Constructs the Window with default values --- |
----------------------------------------------------------------*/
CaledonEngine::Window::Window(const WindowConfig& config)
	: m_title{ config.title }
	, m_width{ config.width }
	, m_height{ config.height }
	, m_isFullscreen{ config.isFullscreen }
	, m_isResizable{ config.isResizable }
	, m_isFocused{ true }
	, m_isMinimized{ false }
	, m_lastError{ }
{
}

/*---------------------------------------------------
| --- GetTitle: Returns the title of the window --- |
---------------------------------------------------*/
const std::string& CaledonEngine::Window::GetTitle() const
{
	return m_title;
}

/*---------------------------------------------------
| --- GetWidth: Returns the width of the window --- |
---------------------------------------------------*/
int CaledonEngine::Window::GetWidth() const
{
	return m_width;
}

/*-----------------------------------------------------
| --- GetHeight: Returns the height of the window --- |
-----------------------------------------------------*/
int CaledonEngine::Window::GetHeight() const
{
	return m_height;
}

/*------------------------------------------------------------------
| --- IsFullscreen: Checks if the window is in fullscreen mode --- |
------------------------------------------------------------------*/
bool CaledonEngine::Window::IsFullscreen() const
{
	return m_isFullscreen;
}

/*---------------------------------------------------------------
| --- IsResizable: Checks if the user can resize the window --- |
---------------------------------------------------------------*/
bool CaledonEngine::Window::IsResizable() const
{
	return m_isResizable;
}

/*----------------------------------------------------
| --- IsFocused: Checks if the window is focused --- |
----------------------------------------------------*/
bool CaledonEngine::Window::IsFocused() const
{
	return m_isFocused;
}

/*--------------------------------------------------------
| --- IsMinimized: Checks if the window is minimized --- |
--------------------------------------------------------*/
bool CaledonEngine::Window::IsMinimized() const
{
	return m_isMinimized;
}

/*----------------------------------------------------------------------
| --- GetLastError:
----------------------------------------------------------------------*/
const std::string& CaledonEngine::Window::GetLastError() const
{
	return m_lastError;
}
