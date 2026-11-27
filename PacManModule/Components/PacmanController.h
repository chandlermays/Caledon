/*------------------------------
| File: PacmanController.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "CaledonEngine/Systems/Scene/Component.h"

#include "PacmanModule/Input/PacmanInputActions.h"

#include <memory>
#include <vector>

namespace PacmanModule
{
	/*-----------------------------------------------------------------------------------
	| --- PacmanController: Turns PacMan's movement input into a direction for      --- |
	| ---                   the MovementComponent on the same GameObject. Only one  --- |
	| ---                   axis moves at a time, like the arcade game: a new       --- |
	| ---                   horizontal input clears the vertical one and vice versa --- |
	-----------------------------------------------------------------------------------*/
	class PacmanController : public CaledonEngine::Component
	{
	private:
		std::shared_ptr<PacmanInputActions> m_pInputActions;								// The PacMan InputActions this controller listens to
		CaledonEngine::InputActionMap* m_pGameplayActionMap;								// Non-owning pointer to the gameplay action map

		std::vector<CaledonEngine::InputAction*> m_subscribedActions;						// The actions this controller subscribed to, so it can unsubscribe

		float m_horizontalInput;															// The latest horizontal input (-1, 0 or 1)
		float m_verticalInput;																// The latest vertical input (-1, 0 or 1)

		void ConfigureInputBindings();														// Subscribes to the MoveHorizontal and MoveVertical actions
		void UnsubscribeAll();																// Unsubscribes from every subscribed action

		void UpdateMovement();																// Passes the input direction to the MovementComponent

	public:
		PacmanController();																	// Constructor
		~PacmanController() override;														// Destructor
		PacmanController(const PacmanController&) = delete;									// Prevent copy-construction
		PacmanController& operator=(const PacmanController&) = delete;						// Prevent copy-assignment
		PacmanController(PacmanController&&) = delete;										// Prevent move-construction
		PacmanController& operator=(PacmanController&&) = delete;							// Prevent move-assignment

		const std::string& GetTypeName() const override;									// Returns the type name of the Component

		void SetInputActions(std::shared_ptr<PacmanInputActions> pInputActions);			// Sets the InputActions to listen to and subscribes to its actions

		void OnMoveHorizontal(float value);													// Moves horizontally (clears vertical, so movement stays on one axis)
		void OnMoveVertical(float value);													// Moves vertically (clears horizontal, so movement stays on one axis)
	};
}