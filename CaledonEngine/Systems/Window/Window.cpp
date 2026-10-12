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
CaledonEngine::Window::Window(const std::string& title, int width, int height, bool isFullscreen)
	: m_title{ title }
	, m_width{ width }
	, m_height{ height }
	, m_isFullscreen{ isFullscreen }
	, m_isFocused{ true }
	, m_isMinimized{ false }
	, m_lastError{ }
{ }

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