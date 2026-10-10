/*------------------------------
| File: InputAction.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Input.h"

#include <string>
#include <vector>
#include <functional>

// This class represents the individual input actions and their action properties (action type, interactions, processors, etc.)

// An InputAction has two types...
// 1. Action Type: A Value action will start and perform when a control moves from its default value and then perform on every value change.
// It will cancel when controls go back to default value. Also, when enabled, a Value action will respond right away to a control's current value.
// It has a Control Type (any, axis, analog, integer, ...)

// 2. Action Type: A Button action will start when a button is pressed and perform when the press threshold is reached. It will cancel when the button is
// going below the release threshold. Also, if a button is already pressed when the action is enabled, the button has to be released first.
// It has an Initial State Check flag (what this does is check controls bound to the action and if they are already actuated (ie. at non-default value),
// the action will immediately be started and performed.)

// Methods to add...
// AddInteraction() - adds an interaction to the action (eg. Hold, Tap, etc.).

namespace CaledonEngine
{
	class InputAction;

	enum class ActionPhase
	{
		kStarted,
		kPerformed,
		kCanceled
	};

	enum class ActionType
	{
		kValue,												// Responds to value changes
		kButton												// Responds to button presses/releases
	};

	enum class ControlType
	{
		kAny,												// Responds to any control type
		kAxis,												// Responds to axis controls
		kVector2											// Responds to 2D vector controls
	};

	struct InputBinding
	{
		KeyCode m_keyCode;									// Key code for keyboard input
		MouseCode m_mouseCode;								// Mouse code for mouse input
		bool m_isMouseButton;								// Flag to indicate if this binding is for a mouse button

		InputBinding(KeyCode key);							// KeyCode Constructor
		InputBinding(MouseCode mouse);						// MouseCode Constructor
	};

	struct CompositeBinding
	{
		std::vector<InputBinding> m_positiveBindings;		// Bindings for positive direction
		std::vector<InputBinding> m_negativeBindings;		// Bindings for negative direction
	};

	struct Context
	{
		InputAction* m_pAction;
		ActionPhase m_phase;
		float m_value;
	};

	using ActionCallback = std::function<void(const Context&)>;

	// Structure storing the bound target object instance alongside the executable callback
	struct CallbackDelegate
	{
		void* m_pTargetInstance{ nullptr };
		ActionCallback m_callback;
	};

	class InputAction
	{
	private:
		std::string m_name;																						// The name of the input action
		ActionType m_actionType;																				// The action type of the input action
		ControlType m_controlType;																				// The control type for the input action
		bool m_initialStateCheck;																				// Flag for initial state check
		bool m_isEnabled;																						// Flag to indicate if the action is enabled
		bool m_wasActive;																						// Flag to indicate if the action was previously active

		std::vector<InputBinding> m_inputBindings;																// The list of input bindings
		std::vector<CompositeBinding> m_compositeBindings;														// The list of composite bindings

		std::vector<CallbackDelegate> m_onStartedCallbacks;														// Callbacks for when the action starts
		std::vector<CallbackDelegate> m_onPerformedCallbacks;													// Callbacks for when the action performs
		std::vector<CallbackDelegate> m_onCanceledCallbacks;													// Callbacks for when the action cancels
		std::vector<CallbackDelegate> m_onValueCallbacks;														// Callbacks for when the action value changes

		void InvokeCallbacks(const std::vector<CallbackDelegate>& callbacks, const Context& context);			// 
		void InvokeStartedCallbacks(const Context& context);													// Invokes all registered started callbacks
		void InvokePerformedCallbacks(const Context& context);													// Invokes all registered performed callbacks
		void InvokeCanceledCallbacks(const Context& context);													// Invokes all registered canceled callbacks
		void InvokeValueCallbacks(const Context& context);														// Invokes all registered value callbacks

	public:
		InputAction(const std::string& name,
			ActionType type = ActionType::kButton);																// Constructor

		~InputAction() = default;																				// Destructor
		InputAction(const InputAction&) = delete;																// Prevent copy-construction
		InputAction& operator=(const InputAction&) = delete;													// Prevent copy-assignment
		InputAction(InputAction&&) = delete;																	// Prevent move-construction
		InputAction& operator=(InputAction&&) = delete;															// Prevent move-assignment

		void SetActionType(ActionType type);																	// Sets the action type of an input action
		void SetControlType(ControlType type);																	// Sets the control type of an input action
		void SetInitialStateCheck(bool flag);																	// Sets the initial state check flag

		void AddBinding(KeyCode key);																			// Adds an input binding to a keyboard input action
		void AddBinding(MouseCode mouse);																		// Adds an input binding to a mouse input action
		void RemoveBinding(KeyCode key);																		// Removes an input binding from a keyboard input action
		void RemoveBinding(MouseCode mouse);																	// Removes an input binding from a mouse input action
		void ClearBindings();																					// Clears all input bindings from the action

		void AddPositiveBinding(KeyCode key);																	// Adds a positive keyboard binding to a composite binding
		void AddNegativeBinding(KeyCode key);																	// Adds a negative keyboard binding to a composite binding
		void AddPositiveBinding(MouseCode mouse);																// Adds a positive mouse binding to a composite binding
		void AddNegativeBinding(MouseCode mouse);																// Adds a negative mouse binding to a composite binding

		template <typename T>
		void OnStarted(T* pInstance, void (T::* pMemberFunc)(const Context&))
		{
			m_onStartedCallbacks.push_back({ pInstance, [pInstance, pMemberFunc](const Context& ctx) { (pInstance->*pMemberFunc)(ctx); } });
		}

		template <typename T>
		void OnPerformed(T* pInstance, void (T::* pMemberFunc)(const Context&))
		{
			m_onPerformedCallbacks.push_back({ pInstance, [pInstance, pMemberFunc](const Context& ctx) { (pInstance->*pMemberFunc)(ctx); } });
		}

		template <typename T>
		void OnCanceled(T* pInstance, void (T::* pMemberFunc)(const Context&))
		{
			m_onCanceledCallbacks.push_back({ pInstance, [pInstance, pMemberFunc](const Context& ctx) { (pInstance->*pMemberFunc)(ctx); } });
		}

		template <typename T>
		void OnValue(T* pInstance, void (T::* pMemberFunc)(const Context&))
		{
			m_onValueCallbacks.push_back({ pInstance, [pInstance, pMemberFunc](const Context& ctx) { (pInstance->*pMemberFunc)(ctx); } });
		}

		// Convenience overload for methods accepting raw float directly (e.g., OnMoveHorizontal(float))
		template <typename T>
		void OnValue(T* pInstance, void (T::* pMemberFunc)(float))
		{
			m_onValueCallbacks.push_back({ pInstance, [pInstance, pMemberFunc](const Context& ctx) { (pInstance->*pMemberFunc)(ctx.m_value); } });
		}

		void Unsubscribe(void* pInstance);																		// Unsubscribes all callbacks bound to a specific object instance pointer
		void Process(float value);																				// Evaluates action state transitions and fires callbacks

		const std::string& GetName() const;																		// Returns the name of the input action
		ActionType GetActionType() const;																		// Returns the action type of the input action
		ControlType GetControlType() const;																		// Returns the control type of the input action
		bool GetInitialStateCheck() const;																		// Returns the initial state check flag
		bool IsEnabled() const;																					// Returns whether the action is enabled
		const std::vector<InputBinding>& GetInputBindings() const;												// Returns the input bindings
		const std::vector<CompositeBinding>& GetCompositeBindings() const;										// Returns the composite bindings
	};
}