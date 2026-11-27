/*------------------------------
| File: EngineManager.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "EngineConfig.h"
#include "EngineContext.h"
#include "ISubsystem.h"
#include "IFixedUpdatable.h"
#include "IUpdatable.h"
#include "IRenderable.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace CaledonEngine
{
	/*------------------------------------------------------------------------------------------------
	| --- EngineManager: Owns every subsystem and runs the main loop. Subsystems are             --- |
	| ---                registered before Initialize, initialized in registration order, and    --- |
	| ---                shut down and destroyed in reverse order. A subsystem is published to   --- |
	| ---                the EngineContext only once it has initialized, so lookups never find a --- |
	| ---                half-built one                                                          --- |
	| ---                                                                                        --- |
	| ---                Each frame runs the fixed steps first (so Update sees the freshest      --- |
	| ---                physics state), then Update, then BeginFrame/Render/EndFrame across     --- |
	| ---                every renderable subsystem                                              --- |
	------------------------------------------------------------------------------------------------*/
	class EngineManager
	{
	private:
		enum class State
		{
			Created,																				// Constructed; subsystems can still be registered
			Initialized,																			// Every subsystem initialized; ready to Run
			Running,																				// Inside the main loop
			Shutdown																				// Every subsystem shut down and destroyed
		};

		struct SubsystemEntry
		{
			std::unique_ptr<ISubsystem> pSubsystem;													// The owned subsystem
			bool isInitialized;																		// Whether the subsystem was initialized (and so must be shut down)
		};

		EngineContext m_context;																	// The handle subsystems use to reach the engine
		std::vector<SubsystemEntry> m_entries;														// Every registered subsystem, in registration order
		std::vector<IFixedUpdatable*> m_pFixedUpdatables;											// Non-owning pointers to the subsystems ticked at the fixed rate
		std::vector<IUpdatable*> m_pUpdatables;														// Non-owning pointers to the subsystems ticked once per frame
		std::vector<IRenderable*> m_pRenderables;													// Non-owning pointers to the subsystems that render each frame
		State m_state;																				// The current lifecycle state of the engine

		bool AddSubsystem(std::unique_ptr<ISubsystem> pSubsystem, IFixedUpdatable* pFixedUpdatable, IUpdatable* pUpdatable, IRenderable* pRenderable);	// Stores a subsystem and the interfaces it implements
		void FixedUpdate(float fixedDeltaTime);														// Ticks every fixed-rate subsystem once
		void Update(float deltaTime);																// Ticks every per-frame subsystem
		void Render();																				// Runs BeginFrame, Render and EndFrame across every renderable subsystem

	public:
		explicit EngineManager(const EngineConfig& config = EngineConfig{});						// Constructor
		~EngineManager();																			// Destructor
		EngineManager(const EngineManager&) = delete;												// Prevent copy-construction
		EngineManager& operator=(const EngineManager&) = delete;									// Prevent copy-assignment
		EngineManager(EngineManager&&) = delete;													// Prevent move-construction
		EngineManager& operator=(EngineManager&&) = delete;											// Prevent move-assignment

		template<typename T>
		bool RegisterSubsystem(std::unique_ptr<T> pSubsystem)										// Takes ownership of a subsystem and records which update interfaces it implements
		{
			static_assert(std::is_base_of_v<ISubsystem, T>, "RegisterSubsystem: T must derive from ISubsystem");

			IFixedUpdatable* pFixedUpdatable = nullptr;
			IUpdatable* pUpdatable = nullptr;
			IRenderable* pRenderable = nullptr;

			if (pSubsystem)
			{
				if constexpr (std::is_base_of_v<IFixedUpdatable, T>)
					pFixedUpdatable = static_cast<IFixedUpdatable*>(pSubsystem.get());

				if constexpr (std::is_base_of_v<IUpdatable, T>)
					pUpdatable = static_cast<IUpdatable*>(pSubsystem.get());

				if constexpr (std::is_base_of_v<IRenderable, T>)
					pRenderable = static_cast<IRenderable*>(pSubsystem.get());
			}

			return AddSubsystem(std::move(pSubsystem), pFixedUpdatable, pUpdatable, pRenderable);
		}

		bool Initialize();																			// Initializes every subsystem in registration order
		void Run();																					// Runs the main loop until a quit is requested
		void RequestQuit() noexcept;																// Asks the main loop to stop at the end of the frame
		void Shutdown() noexcept;																	// Shuts down and destroys every subsystem in reverse order

		EngineContext& Context() noexcept;															// Returns the EngineContext
	};
}
