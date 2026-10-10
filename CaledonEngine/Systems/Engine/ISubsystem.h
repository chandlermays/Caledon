/*------------------------------
| File: ISubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <string_view>

/*---------------------------------------------------------------------------------------------
| ISubsystem: A unit of engine functionality with an explicit, deterministic lifecycle.
|
| Contract (enforced by EngineManager):
| - Initialize() is called once, in registration order. Subsystems registered earlier are
|   already initialized and can be found through the context; later ones are not visible yet.
| - If Initialize() returns false it must leave nothing behind: Shutdown() is NOT called for a
|   subsystem whose Initialize() failed.
| - Shutdown() is called once, in exact reverse order, and only for initialized subsystems.
|   It must not throw.
| - Name() must be unique and stable. It is the lookup key behind EngineContext::Get<T>().
---------------------------------------------------------------------------------------------*/

namespace CaledonEngine
{
	class EngineContext;

	class ISubsystem
	{
	protected:
		ISubsystem() = default;												// Constructor (derived classes only)

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