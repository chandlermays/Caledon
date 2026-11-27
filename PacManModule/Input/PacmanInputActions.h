/*------------------------------
| File: PacmanInputActions.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "CaledonEngine/Systems/Input/InputActions.h"

// This 'PacmanInputActions' class acts as the instance of InputActions for the PacMan Project.
// Here, we can set up the action maps, input actions, and their bindings specific to the Game.

namespace PacmanModule
{
	/*------------------------------------------------------------------------------
	| --- PacmanInputActions: The PacMan game's InputActions: its action maps, --- |
	| ---                     actions and their key bindings                   --- |
	------------------------------------------------------------------------------*/
	class PacmanInputActions : public CaledonEngine::InputActions
	{
	private:
		CaledonEngine::InputActionMap* m_pGameplayActionMap;									// Non-owning pointer to the map of in-game actions (movement, etc.)

		void ConfigureGameplayMap();															// Creates the gameplay actions and their bindings

	public:
		PacmanInputActions();																	// Constructor
		~PacmanInputActions() = default;														// Destructor
		PacmanInputActions(const PacmanInputActions&) = delete;									// Prevent copy-construction
		PacmanInputActions& operator=(const PacmanInputActions&) = delete;						// Prevent copy-assignment
		PacmanInputActions(PacmanInputActions&&) = delete;										// Prevent move-construction
		PacmanInputActions& operator=(PacmanInputActions&&) = delete;							// Prevent move-assignment

		void Initialize();																		// Builds every action map

		CaledonEngine::InputActionMap* GetGameplayActionMap() const;							// Returns the gameplay action map
	};
}