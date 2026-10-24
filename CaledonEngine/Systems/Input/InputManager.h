#pragma once
#include "../Engine/ISubsystem.h"
#include "../Engine/IUpdatable.h"
#include "../Input/Input.h"
#include "../Input/InputActions.h"

#include <memory>
#include <string_view>

namespace CaledonEngine
{
	class EngineContext;

	class InputSubsystem : public ISubsystem, public IUpdatable
	{
	private:
		std::unique_ptr<Input> m_pInputAPI;										//
		InputActions* m_pInputActions;											//
		EngineContext* m_pContext;

		void ProcessInputActions();												//
		void ProcessAction(InputAction* pAction);								//
		float CalculateAxisValue(InputAction* pAction);							//

	public:
		static constexpr std::string_view kName = "Input";						//

		InputSubsystem();														// Constructor
		~InputSubsystem() override = default;									// Destructor
		InputSubsystem(const InputSubsystem&) = delete;							// Prevent copy-construction
		InputSubsystem& operator=(const InputSubsystem&) = delete;				// Prevent copy-assignment
		InputSubsystem(InputSubsystem&&) = delete;								// Prevent move-construction
		InputSubsystem& operator=(InputSubsystem&&) = delete;					// Prevent move-assignment

		std::string_view Name() const noexcept override;						// 
		bool Initialize(EngineContext& context) override;						//
		void Shutdown() noexcept override;										//
		void Update(float deltaTime) override;									//

		bool ProcessEvents();													//

		void SetInputActions(InputActions* pInputActions);						//

		Input* GetInputAPI() const;												//
		InputActions* GetInputActions() const;									//
	};
}