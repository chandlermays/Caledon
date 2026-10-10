/*------------------------------
| File: SDLInput.cpp
| Author: Chandler Mays
------------------------------*/
#include "SDLInput.h"

#include <SDL3/SDL.h>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::SDLInput::~SDLInput()
{
	Shutdown();
}

/*-----------------------------------------------------------
| --- Initialize: Prepares the SDL Input System for use --- |
-----------------------------------------------------------*/
bool CaledonEngine::SDLInput::Initialize()
{
	if (m_isInitialized)
		return true;

	if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
	{
		m_lastError = SDL_GetError();
		return false;
	}

	// Initialize the Key Mappings
	m_keyMapping.emplace(SDLK_UP, KeyCode::kUp);
	m_keyMapping.emplace(SDLK_DOWN, KeyCode::kDown);
	m_keyMapping.emplace(SDLK_LEFT, KeyCode::kLeft);
	m_keyMapping.emplace(SDLK_RIGHT, KeyCode::kRight);
	m_keyMapping.emplace(SDLK_ESCAPE, KeyCode::kEscape);
	m_keyMapping.emplace(SDLK_PAGEUP, KeyCode::kPageUp);
	m_keyMapping.emplace(SDLK_PAGEDOWN, KeyCode::kPageDown);
	m_keyMapping.emplace(SDLK_BACKSPACE, KeyCode::kBackspace);
	m_keyMapping.emplace(SDLK_RETURN, KeyCode::kEnter);
	m_keyMapping.emplace(SDLK_SPACE, KeyCode::kSpace);
	m_keyMapping.emplace(SDLK_LSHIFT, KeyCode::kLeftShift);
	m_keyMapping.emplace(SDLK_RSHIFT, KeyCode::kRightShift);
	m_keyMapping.emplace(SDLK_LCTRL, KeyCode::kLeftCtrl);
	m_keyMapping.emplace(SDLK_RCTRL, KeyCode::kRightCtrl);
	m_keyMapping.emplace(SDLK_LALT, KeyCode::kLeftAlt);
	m_keyMapping.emplace(SDLK_RALT, KeyCode::kRightAlt);
	m_keyMapping.emplace(SDLK_TAB, KeyCode::kTab);
	m_keyMapping.emplace(SDLK_A, KeyCode::kA);
	m_keyMapping.emplace(SDLK_B, KeyCode::kB);
	m_keyMapping.emplace(SDLK_C, KeyCode::kC);
	m_keyMapping.emplace(SDLK_D, KeyCode::kD);
	m_keyMapping.emplace(SDLK_E, KeyCode::kE);
	m_keyMapping.emplace(SDLK_F, KeyCode::kF);
	m_keyMapping.emplace(SDLK_G, KeyCode::kG);
	m_keyMapping.emplace(SDLK_H, KeyCode::kH);
	m_keyMapping.emplace(SDLK_I, KeyCode::kI);
	m_keyMapping.emplace(SDLK_J, KeyCode::kJ);
	m_keyMapping.emplace(SDLK_K, KeyCode::kK);
	m_keyMapping.emplace(SDLK_L, KeyCode::kL);
	m_keyMapping.emplace(SDLK_M, KeyCode::kM);
	m_keyMapping.emplace(SDLK_N, KeyCode::kN);
	m_keyMapping.emplace(SDLK_O, KeyCode::kO);
	m_keyMapping.emplace(SDLK_P, KeyCode::kP);
	m_keyMapping.emplace(SDLK_Q, KeyCode::kQ);
	m_keyMapping.emplace(SDLK_R, KeyCode::kR);
	m_keyMapping.emplace(SDLK_S, KeyCode::kS);
	m_keyMapping.emplace(SDLK_T, KeyCode::kT);
	m_keyMapping.emplace(SDLK_U, KeyCode::kU);
	m_keyMapping.emplace(SDLK_V, KeyCode::kV);
	m_keyMapping.emplace(SDLK_W, KeyCode::kW);
	m_keyMapping.emplace(SDLK_X, KeyCode::kX);
	m_keyMapping.emplace(SDLK_Y, KeyCode::kY);
	m_keyMapping.emplace(SDLK_Z, KeyCode::kZ);
	m_keyMapping.emplace(SDLK_1, KeyCode::k1);
	m_keyMapping.emplace(SDLK_2, KeyCode::k2);
	m_keyMapping.emplace(SDLK_3, KeyCode::k3);
	m_keyMapping.emplace(SDLK_4, KeyCode::k4);
	m_keyMapping.emplace(SDLK_5, KeyCode::k5);
	m_keyMapping.emplace(SDLK_6, KeyCode::k6);
	m_keyMapping.emplace(SDLK_7, KeyCode::k7);
	m_keyMapping.emplace(SDLK_8, KeyCode::k8);
	m_keyMapping.emplace(SDLK_9, KeyCode::k9);
	m_keyMapping.emplace(SDLK_0, KeyCode::k0);
	m_keyMapping.emplace(SDLK_F1, KeyCode::kF1);
	m_keyMapping.emplace(SDLK_F2, KeyCode::kF2);
	m_keyMapping.emplace(SDLK_F3, KeyCode::kF3);
	m_keyMapping.emplace(SDLK_F4, KeyCode::kF4);
	m_keyMapping.emplace(SDLK_F5, KeyCode::kF5);
	m_keyMapping.emplace(SDLK_F6, KeyCode::kF6);
	m_keyMapping.emplace(SDLK_F7, KeyCode::kF7);
	m_keyMapping.emplace(SDLK_F8, KeyCode::kF8);
	m_keyMapping.emplace(SDLK_F9, KeyCode::kF9);
	m_keyMapping.emplace(SDLK_F10, KeyCode::kF10);
	m_keyMapping.emplace(SDLK_F11, KeyCode::kF11);
	m_keyMapping.emplace(SDLK_F12, KeyCode::kF12);
	m_keyMapping.emplace(SDLK_HOME, KeyCode::kHome);
	m_keyMapping.emplace(SDLK_END, KeyCode::kEnd);
	m_keyMapping.emplace(SDLK_INSERT, KeyCode::kInsert);
	m_keyMapping.emplace(SDLK_DELETE, KeyCode::kDelete);
	m_keyMapping.emplace(SDLK_CAPSLOCK, KeyCode::kCapsLock);
	m_keyMapping.emplace(SDLK_NUMLOCKCLEAR, KeyCode::kNumLock);
	m_keyMapping.emplace(SDLK_SCROLLLOCK, KeyCode::kScrollLock);
	m_keyMapping.emplace(SDLK_PRINTSCREEN, KeyCode::kPrintScreen);
	m_keyMapping.emplace(SDLK_PLUS, KeyCode::kPlus);
	m_keyMapping.emplace(SDLK_MINUS, KeyCode::kMinus);
	m_keyMapping.emplace(SDLK_ASTERISK, KeyCode::kMultiply);
	m_keyMapping.emplace(SDLK_SLASH, KeyCode::kDivide);
	m_keyMapping.emplace(SDLK_EQUALS, KeyCode::kEquals);
	m_keyMapping.emplace(SDLK_LEFTBRACKET, KeyCode::kLeftBracket);
	m_keyMapping.emplace(SDLK_RIGHTBRACKET, KeyCode::kRightBracket);
	m_keyMapping.emplace(SDLK_SEMICOLON, KeyCode::kSemicolon);
	m_keyMapping.emplace(SDLK_APOSTROPHE, KeyCode::kApostrophe);
	m_keyMapping.emplace(SDLK_COMMA, KeyCode::kComma);
	m_keyMapping.emplace(SDLK_PERIOD, KeyCode::kPeriod);
	m_keyMapping.emplace(SDLK_BACKSLASH, KeyCode::kBackslash);
	m_keyMapping.emplace(SDLK_GRAVE, KeyCode::kTilde);


	// Initialize the Mouse Mappings
	m_mouseMapping.emplace(SDL_BUTTON_LEFT, MouseCode::kMouseLeft);
	m_mouseMapping.emplace(SDL_BUTTON_RIGHT, MouseCode::kMouseRight);
	m_mouseMapping.emplace(SDL_BUTTON_MIDDLE, MouseCode::kMouseMiddle);
	m_mouseMapping.emplace(SDL_BUTTON_X1, MouseCode::kMouse4);
	m_mouseMapping.emplace(SDL_BUTTON_X2, MouseCode::kMouse5);

	if (m_keyMapping.empty() || m_mouseMapping.empty())
	{
		m_lastError = "SDL key/mouse mappings are empty";
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
		return false;
	}

	m_isInitialized = true;
	return true;
}

/*-----------------------------------------------------------------
| --- Shutdown: Shuts down and cleans up the SDL Input System --- |
-----------------------------------------------------------------*/
void CaledonEngine::SDLInput::Shutdown()
{
	if (!m_isInitialized)
		return;

	m_keyMapping.clear();
	m_mouseMapping.clear();
	m_eventCallback = nullptr;

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	m_isInitialized = false;
}

/*-------------------------------------------------------------------------
| --- ProcessEvents: Processes input events from the SDL Input System --- |
-------------------------------------------------------------------------*/
bool CaledonEngine::SDLInput::ProcessEvents()
{
	m_prevKeyStates = m_keyStates;
	m_prevMouseStates = m_mouseStates;

	m_mouseWheelDelta = 0.0f;

	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		if (m_eventCallback)
		{
			m_eventCallback(&event);
		}

		switch (event.type)
		{
		// Window Event: Closing the Window
		case SDL_EVENT_QUIT:
			return true;

		// Keyboard Event: Key Pressed
		case SDL_EVENT_KEY_DOWN:
		{
			const auto it = m_keyMapping.find(event.key.key);
			if (it != m_keyMapping.end())
			{
				m_keyStates[it->second] = true;
			}
			break;
		}

		// Keyboard Event: Key Released
		case SDL_EVENT_KEY_UP:
		{
			const auto it = m_keyMapping.find(event.key.key);
			if (it != m_keyMapping.end())
			{
				m_keyStates[it->second] = false;
			}
			break;
		}

		// Mouse Event: Mouse Button Pressed
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			const auto it = m_mouseMapping.find(event.button.button);
			if (it != m_mouseMapping.end())
			{
				m_mouseStates[it->second] = true;
			}
			break;
		}

		// Mouse Event: Mouse Button Released
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			const auto it = m_mouseMapping.find(event.button.button);
			if (it != m_mouseMapping.end())
			{
				m_mouseStates[it->second] = false;
			}
			break;
		}

		// Mouse Event: Mouse Motion
		case SDL_EVENT_MOUSE_MOTION:
		{
			m_mousePositionX = event.motion.x;
			m_mousePositionY = event.motion.y;
			break;
		}

		// Mouse Event: Mouse Wheel
		case SDL_EVENT_MOUSE_WHEEL:
		{
			m_mouseWheelDelta = event.wheel.y;
			break;
		}

		// More events, if any
		default:
			break;
		}
	}

	return false;
}

/*-------------------------------------------------------------------------
| --- SetEventCallback:
-------------------------------------------------------------------------*/
void CaledonEngine::SDLInput::SetEventCallback(EventCallback callback)
{
	m_eventCallback = std::move(callback);
}