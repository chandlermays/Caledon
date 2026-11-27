/*------------------------------
| File: ModuleSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "../Engine/ISubsystem.h"
#include "../Module/DynamicLibrary.h"
#include "../Module/ModuleInterface.h"

#include <string>
#include <string_view>
#include <vector>

namespace CaledonEngine
{
	class ComponentRegistry;
	class EngineContext;
	class InputSubsystem;

	/*-------------------------------------------------------------------------------------
	| --- ModuleSubsystem: Loads the game module (.dll) and connects it to the        --- |
	| ---                  engine. The module exports a register and an unregister    --- |
	| ---                  function; on load it is given a Host of callbacks it uses  --- |
	| ---                  to register its Components and hand over its InputActions. --- |
	| ---                  Teardown undoes all of it before the library is unloaded,  --- |
	| ---                  so nothing in the engine points into unloaded code         --- |
	-------------------------------------------------------------------------------------*/
	class ModuleSubsystem : public ISubsystem
	{
	private:
		std::string m_path;																	// The path of the game module library to load
		DynamicLibrary m_library;															// The loaded game module library
		ModuleInterface::Host m_host;														// The callbacks the module uses to register with the engine
		ModuleInterface::UnregisterFunc m_pUnregister;										// The module's unregister function, called on teardown
		EngineContext* m_pContext;															// Non-owning pointer to the EngineContext, set in Initialize
		ComponentRegistry* m_pRegistry;														// Non-owning pointer to the ComponentRegistry the module registers into
		InputSubsystem* m_pInput;															// Non-owning pointer to the InputSubsystem that receives the module's actions
		std::vector<std::string> m_registeredComponents;									// The Component names the module registered, so they can be unregistered
		bool m_hasInputActions;																// Whether the module handed over its InputActions

		static bool HostRegisterComponent(void* user, const char* name, Component* (*create)());	// Host callback: registers one of the module's Components
		static void HostSetInputActions(void* user, InputActions* pActions);				// Host callback: hands the module's InputActions to the InputSubsystem
		void Teardown() noexcept;															// Undoes everything the module registered and unloads it

	public:
		static constexpr std::string_view kName = "Module";									// Unique name used for registration and lookup

		explicit ModuleSubsystem(std::string modulePath);									// Constructor
		~ModuleSubsystem() override = default;												// Destructor
		ModuleSubsystem(const ModuleSubsystem&) = delete;									// Prevent copy-construction
		ModuleSubsystem& operator=(const ModuleSubsystem&) = delete;						// Prevent copy-assignment
		ModuleSubsystem(ModuleSubsystem&&) = delete;										// Prevent move-construction
		ModuleSubsystem& operator=(ModuleSubsystem&&) = delete;								// Prevent move-assignment

		std::string_view Name() const noexcept override;									// Returns the Subsystem's name
		bool Initialize(EngineContext& context) override;									// Loads the module and lets it register with the engine
		void Shutdown() noexcept override;													// Unregisters and unloads the module
	};
}