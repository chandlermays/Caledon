/*------------------------------
| File: ISubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <string_view>

/*---------------------------------------------------------------------------------------------------
| --- ISubsystem:    Defines the contract and deterministic lifecycle for engine subsystems.    --- |
| ---                EngineManager initializes them in registration order and shuts them        --- |
| ---                down in reverse order. Failed initialization must clean up itself;         --- |
| ---                only successfully initialized subsystems are shut down. Name must be       --- |
| ---                unique and stable for lookup through EngineContext.                        --- |
---------------------------------------------------------------------------------------------------*/

namespace CaledonEngine
{
	class EngineContext;

	class ISubsystem
	{
	protected:
		ISubsystem() = default;												// Constructor

	public:
		virtual ~ISubsystem() = default;									// Destructor
		ISubsystem(const ISubsystem&) = delete;								// Prevent copy-construction
		ISubsystem& operator=(const ISubsystem&) = delete;					// Prevent copy-assignment
		ISubsystem(ISubsystem&&) = delete;									// Prevent move-construction
		ISubsystem& operator=(ISubsystem&&) = delete;						// Prevent move-assignment

		virtual std::string_view Name() const noexcept = 0;					// Unique, stable identifier
		virtual bool Initialize(EngineContext& context) = 0;				// Acquire resources; false = failure
		virtual void Shutdown() noexcept = 0;								// Release resources
	};
}