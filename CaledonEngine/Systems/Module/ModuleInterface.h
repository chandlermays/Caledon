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

	namespace ModuleInterface
	{
		inline constexpr std::uint32_t kVersion = 1;

		inline constexpr const char* kRegisterFunctionName = "CaledonModule_Register";
		inline constexpr const char* kUnregisterFunctionName = "CaledonModule_Unregister";

		struct Host
		{
			std::uint32_t version;
			void* user;

			bool (*RegisterComponent)(void* user, const char* name, Component* (*create)());
			void (*SetInputActions)(void* user, InputActions* pActions);
		};

		using RegisterFunc = bool (*)(const Host*);
		using UnregisterFunc = void (*)(const Host*);
	}
}