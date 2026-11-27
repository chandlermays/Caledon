/*------------------------------
| File: EngineContext.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "EngineConfig.h"
#include "ISubsystem.h"

#include <atomic>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace CaledonEngine
{
	class EngineManager;

	/*-----------------------------------------------------------------------------------------------
	| EngineContext: The single handle through which subsystems (and later, game modules) reach the
	| engine. It replaces global/singleton access.
	|
	| Why this exists: CaledonEngine is a static library, so every binary that links it (Editor, Game,
	| Game Module .dll) gets its OWN copy of any global or function-local static. A singleton reached
	| from a module would be a different, empty object. Passing the context in explicitly means every
	| binary talks to the one instance the host created.
	|
	| Lookup is by subsystem name (a string), not by RTTI or per-type static IDs, because both of
	| those are per-binary state and can disagree across a DLL boundary.
	-----------------------------------------------------------------------------------------------*/
	class EngineContext
	{
	private:
		friend class EngineManager;											// Lets the EngineManager construct the context and publish/retract subsystems

		struct PublishedEntry
		{
			std::string name;												// The subsystem's name, used as its lookup key
			ISubsystem* pSubsystem;											// Non-owning pointer to the published subsystem
		};

		EngineConfig m_config;												// The configuration the engine was created with
		std::vector<PublishedEntry> m_published;							// The initialized subsystems that can be looked up by name
		std::atomic<bool> m_quitRequested;									// Flag to indicate the main loop should stop

		explicit EngineContext(const EngineConfig& config);					// Constructor

		void Publish(ISubsystem& subsystem);								// Makes an initialized subsystem available for lookup
		void Retract(ISubsystem& subsystem) noexcept;						// Removes a subsystem from lookup before it shuts down
		void ResetQuit() noexcept;											// Clears the quit request

	public:
		~EngineContext() = default;											// Destructor
		EngineContext(const EngineContext&) = delete;						// Prevent copy-construction
		EngineContext& operator=(const EngineContext&) = delete;			// Prevent copy-assignment
		EngineContext(EngineContext&&) = delete;							// Prevent move-construction
		EngineContext& operator=(EngineContext&&) = delete;					// Prevent move-assignment

		ISubsystem* FindSubsystem(std::string_view name) const noexcept;	// Returns the published subsystem with the given name, or nullptr

		// Typed lookup. T must derive from ISubsystem and expose: static constexpr std::string_view kName.
		template<typename T>
		T* Get() const noexcept
		{
			static_assert(std::is_base_of_v<ISubsystem, T>, "EngineContext::Get<T>: T must derive from ISubsystem");
			return static_cast<T*>(FindSubsystem(T::kName));
		}

		const EngineConfig& Config() const noexcept;						// Returns the engine configuration

		void RequestQuit() noexcept;										// Asks the main loop to stop at the end of the frame
		bool IsQuitRequested() const noexcept;								// Returns whether a quit has been requested
	};
}
