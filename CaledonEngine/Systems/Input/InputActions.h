/*------------------------------
| File: InputActions.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "InputActionMap.h"

#include <unordered_map>
#include <string>

// This is the class (or asset) that gets created in the Game project to define the input actions and their mappings to keys/buttons.

// Contains the action maps (eg. Gameplay, UI, etc.) and the input actions within those maps.

namespace CaledonEngine
{
	class InputActions
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<InputActionMap>> m_actionMaps;								// The map of input action maps
		std::string m_assetName;																					// The name of the input actions asset
		bool m_isEnabled;																							// Flag to indicate if the input actions are enabled

	public:
		InputActions(const std::string& assetName = "NewInputActions");												// Constructor
		virtual ~InputActions() = default;																			// Destructor
		InputActions(const InputActions&) = delete;																	// Prevent copy-construction
		InputActions& operator=(const InputActions&) = delete;														// Prevent copy-assignment
		InputActions(InputActions&&) = delete;																		// Prevent move-construction
		InputActions& operator=(InputActions&&) = delete;															// Prevent move-assignment

		void AddActionMap(const std::string& name);																	// Adds an input action map to the asset
		void RemoveActionMap(const std::string& name);																// Removes an input action map from the asset

		void Enable();																								// Enables the input actions
		void Disable();																								// Disables the input actions
		bool Contains(InputAction* action) const;																	// Checks if the input actions contain a specific action	

		const std::unordered_map<std::string, std::unique_ptr<InputActionMap>>& GetInputActionMaps() const;			// Returns the map of input action maps (const)
		const InputActionMap* GetActionMapByName(const std::string& name) const;									// Returns an input action map by name (const)
		InputActionMap* GetActionMapByName(const std::string& name);												// Returns an input action map by name
		InputAction* GetActionByName(const std::string& name) const;												// Returns an input action by name
		bool IsEnabled() const;																						// Checks if the input actions are enabled
	};
}