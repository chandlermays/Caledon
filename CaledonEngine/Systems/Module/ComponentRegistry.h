/*------------------------------
| File: ComponentRegistry.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "../Engine/ISubsystem.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace CaledonEngine
{
	class Component;

	/*-------------------------------------------------------------------------------------
	| --- ComponentRegistry: Maps Component type names to functions that create       --- |
	| ---                    them, so the engine can build Components it doesn't know --- |
	| ---                    at compile time (e.g. the game module's) from a name     --- |
	-------------------------------------------------------------------------------------*/
	class ComponentRegistry : public ISubsystem
	{
	public:
		using CreateFunction = Component * (*)();											// Function that creates a new Component of one type

	private:
		std::unordered_map<std::string, CreateFunction> m_creators;							// The create function for each registered Component name

	public:
		static constexpr std::string_view kName = "ComponentRegistry";						// Unique name used for registration and lookup

		ComponentRegistry() = default;														// Constructor
		~ComponentRegistry() override = default;											// Destructor
		ComponentRegistry(const ComponentRegistry&) = delete;								// Prevent copy-construction
		ComponentRegistry& operator=(const ComponentRegistry&) = delete;					// Prevent copy-assignment
		ComponentRegistry(ComponentRegistry&&) = delete;									// Prevent move-construction
		ComponentRegistry& operator=(ComponentRegistry&&) = delete;							// Prevent move-assignment

		std::string_view Name() const noexcept override;									// Returns the Subsystem's name
		bool Initialize(EngineContext& context) override;									// Initializes the Subsystem (nothing to set up)
		void Shutdown() noexcept override;													// Forgets every registered Component

		bool Register(std::string_view name, CreateFunction create);						// Registers a Component type by name; false if the name is taken
		bool Unregister(std::string_view name) noexcept;									// Removes a Component type; false if it wasn't registered
		bool IsRegistered(std::string_view name) const;										// Returns whether a Component type is registered
		std::unique_ptr<Component> Create(std::string_view name) const;						// Creates a Component by name, or nullptr if unknown
	};
}