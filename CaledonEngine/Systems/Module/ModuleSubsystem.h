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
	class InputManager;

	class ModuleSubsystem : public ISubsystem
	{
	private:
		std::string m_path;
		DynamicLibrary m_library;
		ModuleInterface::Host m_host;
		ModuleInterface::UnregisterFunc m_pUnregister;
		EngineContext* m_pContext;
		ComponentRegistry* m_pRegistry;
		InputManager* m_pInput;
		std::vector<std::string> m_registeredComponents;
		bool m_hasInputActions;

		static bool HostRegisterComponent(void* user, const char* name, Component* (*create)());
		static void HostSetInputActions(void* user, InputActions* pActions);
		void Teardown() noexcept;

	public:
		static constexpr std::string_view kName = "Module";

		explicit ModuleSubsystem(std::string modulePath);									// Constructor
		~ModuleSubsystem() override = default;												// Destructor
		ModuleSubsystem(const ModuleSubsystem&) = delete;									// Prevent copy-construction
		ModuleSubsystem& operator=(const ModuleSubsystem&) = delete;						// Prevent copy-assignment
		ModuleSubsystem(ModuleSubsystem&&) = delete;										// Prevent move-construction
		ModuleSubsystem& operator=(ModuleSubsystem&&) = delete;								// Prevent move-assignment

		std::string_view Name() const noexcept override;
		bool Initialize(EngineContext& context) override;
		void Shutdown() noexcept override;
	};
}