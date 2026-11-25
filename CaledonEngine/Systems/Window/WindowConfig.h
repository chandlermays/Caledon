/*------------------------------
| File: WindowConfig.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <string>

namespace CaledonEngine
{
	/*--------------------------------------------------------------------------------------
	| --- WindowConfig: How the WindowSubsystem opens its window. The defaults are the --- |
	| ---               same values the Window used before, so the Game is unchanged   --- |
	--------------------------------------------------------------------------------------*/
	struct WindowConfig
	{
		std::string title = "Caledon Engine";								// The title of the window
		int width = 1280;													// The width of the window, in pixels
		int height = 720;													// The height of the window, in pixels
		bool isFullscreen = false;											// Opens the window in fullscreen mode
		bool isResizable = false;											// Lets the user resize the window by dragging its edges
	};
}
