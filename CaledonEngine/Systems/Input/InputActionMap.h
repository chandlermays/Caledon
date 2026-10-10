/*------------------------------
| File: InputActionMap.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "InputAction.h"

#include <unordered_map>
#include <string>
#include <memory>

// This class represents the map (or container) of Input Actions, typically associated with a specific context (eg. Gameplay, UI, etc.)

namespace CaledonEngine
{
	class InputActionMap
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<InputAction>> m_actions;						// The map of input actions
		std::string m_name;																				// The name of the action map
		bool m_isEnabled;																				// Flag to indicate if the action map is enabled

	public:
		InputActionMap(const std::string& name);														// Constructor
		~InputActionMap() = default;																	// Destructor
		InputActionMap(const InputActionMap&) = delete;													// Prevent copy-construction
		InputActionMap& operator=(const InputActionMap&) = delete;										// Prevent copy-assignment
		InputActionMap(InputActionMap&&) = delete;														// Prevent move-construction
		InputActionMap& operator=(InputActionMap&&) = delete;											// Prevent move-assignment

		void AddAction(const std::string& name, ActionType type = ActionType::kButton);					// Adds an input action to the map
		void RemoveAction(const std::string& name);														// Removes an input action from the map

		void Enable();																					// Enables the action map
		void Disable();																					// Disables the action map
		bool Contains(const std::string& name) const;													// Checks if the action map contains an action by name

		const std::unordered_map<std::string, std::unique_ptr<InputAction>>& GetInputActions() const;	// Returns the map of input actions
		InputAction* GetActionByName(const std::string& name) const;									// Returns an input action by name
		const std::string& GetName() const;																// Returns the name of the action map
		bool IsEnabled() const;																			// Checks if the action map is enabled
	};
}