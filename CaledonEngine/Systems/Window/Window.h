/*------------------------------
| File: Window.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <string>

namespace CaledonEngine
{
	class Window
	{
	protected:
		std::string m_title;													// The title of the window
		int m_width;															// The width of the window
		int m_height;															// The height of the window
		bool m_isFullscreen;													// Flag to indicate if the window is in fullscreen mode
		bool m_isFocused;														// Flag to indicate if the window is focused
		bool m_isMinimized;														// Flag to indicate if the window is minimized
		std::string m_lastError;												//

	public:
		Window(const std::string& title = "Caledon Engine",
			int width = 1280, int height = 720, bool isFullscreen = false);		// Constructor

		virtual ~Window() = default;											// Destructor
		Window(const Window&) = delete;											// Prevent copy-construction
		Window& operator=(const Window&) = delete;								// Prevent copy-assignment
		Window(Window&&) = delete;												// Prevent move-construction
		Window& operator=(Window&&) = delete;									// Prevent move-assignment

		virtual bool Initialize() = 0;											// Prepares the Window for use
		virtual void Shutdown() = 0;											// Cleans up resources used by the Window

		virtual void SetTitle(const std::string& title) = 0;					// Sets the title of the window
		virtual void SetSize(int width, int height) = 0;						// Sets the size of the window
		virtual void SetFullscreen(bool isFullscreen) = 0;						// Sets the fullscreen mode of the window
		virtual void Show() = 0;												// Shows the window
		virtual void Hide() = 0;												// Hides the window

		virtual void* GetNativeHandle() const = 0;								// Returns the native window handle

		virtual const std::string& GetTitle() const;							// Returns the title of the window
		virtual int GetWidth() const;											// Returns the width of the window
		virtual int GetHeight() const;											// Returns the height of the window
		virtual bool IsFullscreen() const;										// Checks if the window is in fullscreen mode
		virtual bool IsFocused() const;											// Checks if the window is focused
		virtual bool IsMinimized() const;										// Checks if the window is minimized
		const std::string& GetLastError() const;								//
	};
}