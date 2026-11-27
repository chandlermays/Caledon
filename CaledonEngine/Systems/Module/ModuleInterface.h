/*------------------------------
| File: ModuleInterface.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

#if defined(_WIN32)
#define CALEDON_MODULE_API extern "C" __declspec(dllexport)
#else
#define CALEDON_MODULE_API extern "C" __attribute__((visibility("default")))
#endif

namespace CaledonEngine
{
	class Component;
	class InputActions;

	/*---------------------------------------------------------------------------------
	| --- ModuleInterface: The contract between the engine and a game module. It  --- |
	| ---                  uses only C-compatible types and function pointers, so --- |
	| ---                  it holds across the DLL boundary                       --- |
	---------------------------------------------------------------------------------*/
	namespace ModuleInterface
	{
		inline constexpr std::uint32_t kVersion = 1;												// Version of the Host layout; bumped when it changes

		inline constexpr const char* kRegisterFunctionName = "CaledonModule_Register";				// Name of the function a module exports to register
		inline constexpr const char* kUnregisterFunctionName = "CaledonModule_Unregister";			// Name of the function a module exports to unregister

		struct Host
		{
			std::uint32_t version;																	// The Host layout version (kVersion)
			void* user;																				// Opaque pointer handed back to every callback

			bool (*RegisterComponent)(void* user, const char* name, Component* (*create)());		// Registers one of the module's Components
			void (*SetInputActions)(void* user, InputActions* pActions);							// Hands the module's InputActions to the engine
		};

		using RegisterFunc = bool (*)(const Host*);													// Signature of a module's register function
		using UnregisterFunc = void (*)(const Host*);												// Signature of a module's unregister function
	}
}