/*------------------------------
| File: SDLWindow.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Window/Window.h"

struct SDL_Window;

namespace CaledonEngine
{
	class SDLWindow : public Window
	{
	private:
		SDL_Window* m_pWindow;												// Pointer to the SDL Window

	public:
		SDLWindow();														// Constructor
		~SDLWindow() override;												// Destructor
		SDLWindow(const SDLWindow&) = delete;								// Prevent copy-construction
		SDLWindow& operator=(const SDLWindow&) = delete;					// Prevent copy-assignment
		SDLWindow(SDLWindow&&) = delete;									// Prevent move-construction
		SDLWindow& operator=(SDLWindow&&) = delete;							// Prevent move-assignment

		bool Initialize() override;											// Prepares the SDL Window for use
		void Shutdown() noexcept override;									// Cleans up and shuts down the SDL Window

		void SetTitle(const std::string& title) override;					// Sets the title of the window
		void SetSize(int width, int height) override;						// Sets the size of the window
		void SetFullscreen(bool isFullscreen) override;						// Sets the fullscreen state
		void Show() override;												// Shows the window
		void Hide() override;												// Hides the window

		void* GetNativeHandle() const override;								// Returns the native window handle
	};
}