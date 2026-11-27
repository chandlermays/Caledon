#pragma once
#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IUpdatable.h"
#include "Systems/Input/Input.h"
#include "Systems/Input/InputActions.h"

#include <memory>
#include <string_view>

namespace CaledonEngine
{
	class EngineContext;

	/*------------------------------------------------------------------------------------
	| --- InputSubsystem: Reads input through the platform backend once per frame    --- |
	| ---                 and drives the active InputActions from it. A quit event   --- |
	| ---                 from the OS (e.g. the window closing) asks the engine to   --- |
	| ---                 stop. The InputActions are owned by the game module, which --- |
	| ---                 hands them over through the ModuleSubsystem                --- |
	------------------------------------------------------------------------------------*/
	class InputSubsystem : public ISubsystem, public IUpdatable
	{
	private:
		std::unique_ptr<Input> m_pInputAPI;										// The platform backend that reads the keyboard and mouse
		InputActions* m_pInputActions;											// Non-owning pointer to the active InputActions (set by the game module)
		EngineContext* m_pContext;												// Non-owning pointer to the EngineContext, set in Initialize

		void ProcessInputActions();												// Processes every action in every enabled InputActionMap
		void ProcessAction(InputAction* pAction);								// Reads an action's bindings and passes its value to the action
		float CalculateAxisValue(InputAction* pAction);							// Combines an action's positive and negative bindings into a value in [-1, 1]

	public:
		static constexpr std::string_view kName = "Input";						// Unique name used for registration and lookup

		InputSubsystem();														// Constructor
		~InputSubsystem() override = default;									// Destructor
		InputSubsystem(const InputSubsystem&) = delete;							// Prevent copy-construction
		InputSubsystem& operator=(const InputSubsystem&) = delete;				// Prevent copy-assignment
		InputSubsystem(InputSubsystem&&) = delete;								// Prevent move-construction
		InputSubsystem& operator=(InputSubsystem&&) = delete;					// Prevent move-assignment

		std::string_view Name() const noexcept override;						// Returns the Subsystem's name
		bool Initialize(EngineContext& context) override;						// Starts the platform input backend
		void Shutdown() noexcept override;										// Shuts down the platform input backend
		void Update(float deltaTime) override;									// Processes events and requests a quit when the OS asks for one

		bool ProcessEvents();													// Processes events and actions; returns true if a quit was requested

		void SetInputActions(InputActions* pInputActions);						// Sets the InputActions to process (nullptr to clear)

		Input* GetInputAPI() const;												// Returns the platform input backend
		InputActions* GetInputActions() const;									// Returns the active InputActions
	};
}