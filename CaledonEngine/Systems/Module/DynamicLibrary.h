/*------------------------------
| File: ComponentRegistry.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <string>

namespace CaledonEngine
{
	class DynamicLibrary
	{
	private:
		void* m_pHandle;
		unsigned long m_lastError;

	public:
		DynamicLibrary();																	// Constructor
		~DynamicLibrary();																	// Destructor
		DynamicLibrary(const DynamicLibrary&) = delete;										// Prevent copy-construction
		DynamicLibrary& operator=(const DynamicLibrary&) = delete;							// Prevent copy-assignment
		DynamicLibrary(DynamicLibrary&&) = delete;											// Prevent move-construction
		DynamicLibrary& operator=(DynamicLibrary&&) = delete;								// Prevent move-assignment

		bool Load(const std::string& path);
		void Unload();
		bool IsLoaded() const;
		unsigned long GetLastError() const;

		void* GetFunctionAddress(const std::string& functionName) const;

		template<typename FuncPtr>
		FuncPtr GetFunction(const std::string& functionName) const
		{
			return reinterpret_cast<FuncPtr>(GetFunctionAddress(functionName));
		}
	};
}