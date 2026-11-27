/*------------------------------
| File: SDLWindow.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Window/Window.h"

struct SDL_Window;

namespace CaledonEngine
{
	/*----------------------------------------------------------------------------------------------
	| --- SDLWindow: Implements the engine's window interface using SDL.                       --- |
	| ---            Manages native window creation, configuration, visibility, and sizing.    --- |
	----------------------------------------------------------------------------------------------*/
	class SDLWindow : public Window
	{
	private:
		SDL_Window* m_pWindow;													// Pointer to the SDL Window

	public:
		explicit SDLWindow(const WindowConfig& config = WindowConfig{});		// Constructor
		~SDLWindow() override;													// Destructor
		SDLWindow(const SDLWindow&) = delete;									// Prevent copy-construction
		SDLWindow& operator=(const SDLWindow&) = delete;						// Prevent copy-assignment
		SDLWindow(SDLWindow&&) = delete;										// Prevent move-construction
		SDLWindow& operator=(SDLWindow&&) = delete;								// Prevent move-assignment

		bool Initialize() override;												// Prepares the SDL Window for use
		void Shutdown() noexcept override;										// Cleans up and shuts down the SDL Window

		void SetTitle(const std::string& title) override;						// Sets the title of the window
		void SetSize(int width, int height) override;							// Sets the size of the window
		void SetFullscreen(bool isFullscreen) override;							// Sets the fullscreen state
		void Show() override;													// Shows the window and updates its cached state
		void Hide() override;													// Hides the window and updates its cached state

		void* GetNativeHandle() const override;									// Returns the native window handle

		int GetWidth() const override;											// Returns the current window width so user resizing is reflected accurately
		int GetHeight() const override;											// Returns the current window height so user resizing is reflected accurately
