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

	/*------------------------------------------------------------------------------------------------
	| --- SDLInput: Polls SDL events and translates keyboard and mouse input into engine state.  --- |
	| ---            Can also forward raw SDL events through an optional callback.               --- |
	------------------------------------------------------------------------------------------------*/
	class SDLInput : public Input
	{
	private:
		EventCallback m_eventCallback;												// The function called for every raw SDL event (e.g. to feed ImGui)
		bool m_isInitialized = false;												// Flag to indicate the SDL input subsystem has started

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

		void SetEventCallback(EventCallback callback) override;						// Sets the function called for every raw SDL event
	};
}