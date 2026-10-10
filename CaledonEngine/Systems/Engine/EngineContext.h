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
		friend class EngineManager;											//

		struct PublishedEntry
		{
			std::string name;												//
			ISubsystem* pSubsystem;											//
		};

		EngineConfig m_config;												//
		std::vector<PublishedEntry> m_published;							//
		std::atomic<bool> m_quitRequested;									//

		explicit EngineContext(const EngineConfig& config);					// Constructor

		void Publish(ISubsystem& subsystem);								//
		void Retract(ISubsystem& subsystem) noexcept;						//
		void ResetQuit() noexcept;											//

	public:
		~EngineContext() = default;											// Destructor
		EngineContext(const EngineContext&) = delete;						// Prevent copy-construction
		EngineContext& operator=(const EngineContext&) = delete;			// Prevent copy-assignment
		EngineContext(EngineContext&&) = delete;							// Prevent move-construction
		EngineContext& operator=(EngineContext&&) = delete;					// Prevent move-assignment

		ISubsystem* FindSubsystem(std::string_view name) const noexcept;	//

		// Typed lookup. T must derive from ISubsystem and expose: static constexpr std::string_view kName.
		template<typename T>
		T* Get() const noexcept
		{
			static_assert(std::is_base_of_v<ISubsystem, T>, "EngineContext::Get<T>: T must derive from ISubsystem");
			return static_cast<T*>(FindSubsystem(T::kName));
		}

		const EngineConfig& Config() const noexcept;						//

		void RequestQuit() noexcept;										//
		bool IsQuitRequested() const noexcept;								//
	};
}
