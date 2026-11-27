/*------------------------------
| File: DynamicLibrary.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <string>

namespace CaledonEngine
{
	/*---------------------------------------------------------------------------------
	| --- DynamicLibrary: Loads a shared library (.dll) and looks up its exported --- |
	| ---                 functions. The library is unloaded on destruction       --- |
	---------------------------------------------------------------------------------*/
	class DynamicLibrary
	{
	private:
		void* m_pHandle;																	// The OS handle of the loaded library (nullptr when none)
		unsigned long m_lastError;															// The OS error code from the last failed load

	public:
		DynamicLibrary();																	// Constructor
		~DynamicLibrary();																	// Destructor
		DynamicLibrary(const DynamicLibrary&) = delete;										// Prevent copy-construction
		DynamicLibrary& operator=(const DynamicLibrary&) = delete;							// Prevent copy-assignment
		DynamicLibrary(DynamicLibrary&&) = delete;											// Prevent move-construction
		DynamicLibrary& operator=(DynamicLibrary&&) = delete;								// Prevent move-assignment

		bool Load(const std::string& path);													// Loads a library, unloading any current one first
		void Unload();																		// Unloads the library, if one is loaded
		bool IsLoaded() const;																// Returns whether a library is loaded
		unsigned long GetLastError() const;													// Returns the OS error code from the last failed load

		void* GetFunctionAddress(const std::string& functionName) const;					// Returns the address of an exported function, or nullptr

		template<typename FuncPtr>
		FuncPtr GetFunction(const std::string& functionName) const							// Returns an exported function cast to the given pointer type
		{
			return reinterpret_cast<FuncPtr>(GetFunctionAddress(functionName));
		}
	};
}