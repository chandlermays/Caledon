/*------------------------------
| File: PacmanController.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "CaledonEngine/Systems/Scene/Component.h"

#include "Input/PacmanInputActions.h"

namespace PacmanModule
{
	class PacmanController : public CaledonEngine::Component
	{
	private:
		PacmanInputActions* m_pInputActions;
		CaledonEngine::InputActionMap* m_pGameplayActionMap;

		std::vector<CaledonEngine::InputAction*> m_subscribedActions;

		float m_moveSpeed;
		float m_horizontalInput;
		float m_verticalInput;

		void ConfigureInputBindings();

	public:
		PacmanController();																	// Constructor
		~PacmanController() override;														// Destructor
		PacmanController(const PacmanController&) = delete;									// Prevent copy-construction
		PacmanController& operator=(const PacmanController&) = delete;						// Prevent copy-assignment
		PacmanController(PacmanController&&) = delete;										// Prevent move-construction
		PacmanController& operator=(PacmanController&&) = delete;							// Prevent move-assignment

		const std::string& GetTypeName() const override;

		void SetInputActions(PacmanInputActions* pInputActions);

		void OnMoveHorizontal(float value);
		void OnMoveVertical(float value);

		void SetMoveSpeed(float speed);
		float GetMoveSpeed() const;
	};
}