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
	class PacmanController : public CaledonEngine::Component
	{
	private:
		std::shared_ptr<PacmanInputActions> m_pInputActions;								//
		CaledonEngine::InputActionMap* m_pGameplayActionMap;								//

		std::vector<CaledonEngine::InputAction*> m_subscribedActions;						//

		float m_horizontalInput;															//
		float m_verticalInput;																//

		void ConfigureInputBindings();														//
		void UnsubscribeAll();																//

		void UpdateMovement();																//

	public:
		PacmanController();																	// Constructor
		~PacmanController() override;														// Destructor
		PacmanController(const PacmanController&) = delete;									// Prevent copy-construction
		PacmanController& operator=(const PacmanController&) = delete;						// Prevent copy-assignment
		PacmanController(PacmanController&&) = delete;										// Prevent move-construction
		PacmanController& operator=(PacmanController&&) = delete;							// Prevent move-assignment

		const std::string& GetTypeName() const override;									// Returns the type name of the Component

		void SetInputActions(std::shared_ptr<PacmanInputActions> pInputActions);			//

		void OnMoveHorizontal(float value);													//
		void OnMoveVertical(float value);													//
	};
}