/*------------------------------
| File: SDLInput.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Input/Input.h"

namespace CaledonEngine
{
	typedef int SDL_KeyCode;
	typedef int SDL_MouseCode;

	class SDLInput : public Input
	{
	private:
		EventCallback m_eventCallback;												//
		bool m_isInitialized = false;												//

		std::unordered_map<SDL_KeyCode, KeyCode> m_keyMapping;						// Mapping from SDL Key Codes to Engine Key Codes
		std::unordered_map<SDL_MouseCode, MouseCode> m_mouseMapping;				// Mapping from SDL Mouse Codes to Engine Mouse Codes

	public:
		SDLInput() = default;														// Constructor
		~SDLInput() override;														// Destructor
		SDLInput(const SDLInput&) = delete;											// Prevent copy-construction
		SDLInput& operator=(const SDLInput&) = delete;								// Prevent copy-assignment
		SDLInput(SDLInput&&) = delete;												// Prevent move-construction
		SDLInput& operator=(SDLInput&&) = delete;									// Prevent move-assignment

		bool Initialize() override;													// Prepares the SDL Input System for use
		void Shutdown() override;													// Shuts down and cleans up the SDL Input System
		bool ProcessEvents() override;												// Processes input events from the SDL Input System

		void SetEventCallback(EventCallback callback) override;						//
	};
}