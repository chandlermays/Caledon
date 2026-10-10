/*------------------------------
| File: EngineManager.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "EngineConfig.h"
#include "EngineContext.h"
#include "ISubsystem.h"
#include "IUpdatable.h"
#include "IRenderable.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace CaledonEngine
{
	class EngineManager
	{
	private:
		enum class State
		{
			Created,
			Initialized,
			Running,
			Shutdown
		};

		struct SubsystemEntry
		{
			std::unique_ptr<ISubsystem> pSubsystem;
			bool isInitialized;
		};

		EngineContext m_context;
		std::vector<SubsystemEntry> m_entries;
		std::vector<IUpdatable*> m_pUpdatables;
		std::vector<IRenderable*> m_pRenderables;
		State m_state;

		bool AddSubsystem(std::unique_ptr<ISubsystem> pSubsystem, IUpdatable* pUpdatable, IRenderable* pRenderable);
		void Update(float deltaTime);
		void Render();

	public:
		explicit EngineManager(const EngineConfig& config = EngineConfig{});						// Constructor
		~EngineManager();																			// Destructor
		EngineManager(const EngineManager&) = delete;												// Prevent copy-construction
		EngineManager& operator=(const EngineManager&) = delete;									// Prevent copy-assignment
		EngineManager(EngineManager&&) = delete;													// Prevent move-construction
		EngineManager& operator=(EngineManager&&) = delete;											// Prevent move-assignment

		template<typename T>
		bool RegisterSubsystem(std::unique_ptr<T> pSubsystem)
		{
			static_assert(std::is_base_of_v<ISubsystem, T>, "RegisterSubsystem: T must derive from ISubsystem");

			IUpdatable* pUpdatable = nullptr;
			IRenderable* pRenderable = nullptr;

			if (pSubsystem)
			{
				if constexpr (std::is_base_of_v<IUpdatable, T>)
					pUpdatable = static_cast<IUpdatable*>(pSubsystem.get());

				if constexpr (std::is_base_of_v<IRenderable, T>)
					pRenderable = static_cast<IRenderable*>(pSubsystem.get());
			}

			return AddSubsystem(std::move(pSubsystem), pUpdatable, pRenderable);
		}

		bool Initialize();
		void Run();
		void RequestQuit() noexcept;
		void Shutdown() noexcept;

		EngineContext& Context() noexcept;
	};
}
