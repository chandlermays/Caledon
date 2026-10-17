/*------------------------------
| File: EngineManager.cpp
| Author: Chandler Mays
------------------------------*/
#include "EngineManager.h"

#include "../Engine/FixedTimestep.h"
#include "../Engine/LoggingSubsystem.h"

#include <algorithm>
#include <chrono>
#include <string>
#include <thread>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------
| --- Constructor: Constructs the EngineManager with the given config --- |
-------------------------------------------------------------------------*/
CaledonEngine::EngineManager::EngineManager(const EngineConfig& config)
	: m_context{ config }
	, m_entries{ }
	, m_pFixedUpdatables{ }
	, m_pUpdatables{ }
	, m_pRenderables{ }
	, m_state{ State::Created }
{}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::EngineManager::~EngineManager()
{
	Shutdown();
}

/*---------------------------------------------------------------------------------------
| --- Initialize:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::EngineManager::Initialize()
{
	if (m_state != State::Created)
		return false;

	m_context.ResetQuit();

	try
	{
		for (SubsystemEntry& entry : m_entries)
		{
			[[maybe_unused]] const std::string_view name = entry.pSubsystem->Name();

			if (!entry.pSubsystem->Initialize(m_context))
			{
				CE_LOG(m_context, LogLevel::Error, "Engine", "Failed to initialize subsystem '{}'", name);
				Shutdown();
				return false;
			}

			// Flag first: from here on the subsystem MUST be shut down, even if Publish() throws.
			entry.isInitialized = true;
			m_context.Publish(*entry.pSubsystem);

			CE_LOG(m_context, LogLevel::Log, "Engine", "Initialized subsystem '{}'", name);
		}
	}
	catch (...)
	{
		Shutdown();
		throw;
	}

	m_state = State::Initialized;
	return true;
}

/*---------------------------------------------------------------------------------------
| --- Run:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineManager::Run()
{
	if (m_state != State::Initialized)
	{
		CE_LOG(m_context, LogLevel::Error, "Engine", "Run() ignored: engine is not in the Initialized state");
		return;
	}

	using Clock = std::chrono::steady_clock;

	const EngineConfig& config = m_context.Config();
	const Clock::duration targetFrameTime = (config.targetFps > 0)
		? std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(1.0 / config.targetFps))
		: Clock::duration::zero();

	// Fixed-rate simulation (physics). Disabled entirely when fixedUpdateHz is 0.
	FixedTimestep fixedTimestep{ (config.fixedUpdateHz > 0) ? 1.0 / config.fixedUpdateHz : 0.0, static_cast<int>(config.maxFixedStepsPerFrame) };
	const float fixedDeltaTime = static_cast<float>(fixedTimestep.Step());

	m_state = State::Running;

	std::uint64_t frameCount = 0;
	Clock::time_point lastFrameStart = Clock::now();

	while (!m_context.IsQuitRequested())
	{
		const Clock::time_point frameStart = Clock::now();
		const float rawDeltaTime = std::chrono::duration<float>(frameStart - lastFrameStart).count();
		lastFrameStart = frameStart;

		const float deltaTime = std::min(rawDeltaTime, config.maxFrameTimeSeconds);

		// Fixed steps first, so Update sees the freshest physics state and anything it changes
		// (velocities, forces) is simulated on the next fixed step.
		const int fixedSteps = fixedTimestep.Advance(deltaTime);
		for (int i = 0; i < fixedSteps && !m_context.IsQuitRequested(); ++i)
		{
			FixedUpdate(fixedDeltaTime);
		}

		Update(deltaTime);
		Render();

		++frameCount;
		if (config.maxFrames != 0 && frameCount >= config.maxFrames)
			m_context.RequestQuit();

		if (targetFrameTime > Clock::duration::zero() && !m_context.IsQuitRequested())
			std::this_thread::sleep_until(frameStart + targetFrameTime);
	}

	m_state = State::Initialized;
}

/*---------------------------------------------------------------------------------------
| --- RequestQuit:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineManager::RequestQuit() noexcept
{
	m_context.RequestQuit();
}

/*---------------------------------------------------------------------------------------
| --- Shutdown:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineManager::Shutdown() noexcept
{
	if (m_state == State::Shutdown)
		return;

	m_state = State::Shutdown;

	// These point into the entries below, so they go first.
	m_pFixedUpdatables.clear();
	m_pUpdatables.clear();
	m_pRenderables.clear();

	// pop_back (rather than clear()) because vector destruction order is unspecified;
	// subsystems must be destroyed in reverse registration order too.
	while (!m_entries.empty())
	{
		SubsystemEntry& entry = m_entries.back();

		if (entry.isInitialized)
		{
			[[maybe_unused]] const std::string_view name = entry.pSubsystem->Name();

			CE_LOG(m_context, LogLevel::Log, "Engine", "Shutting down subsystem '{}'", name);
			m_context.Retract(*entry.pSubsystem);
			entry.pSubsystem->Shutdown();
		}

		m_entries.pop_back();
	}
}

/*---------------------------------------------------------------------------------------
| --- Context:
---------------------------------------------------------------------------------------*/
CaledonEngine::EngineContext& CaledonEngine::EngineManager::Context() noexcept
{
	return m_context;
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*---------------------------------------------------------------------------------------
| --- AddSubsystem:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::EngineManager::AddSubsystem(std::unique_ptr<ISubsystem> pSubsystem, IFixedUpdatable* pFixedUpdatable, IUpdatable* pUpdatable, IRenderable* pRenderable)
{
	if (m_state != State::Created || !pSubsystem)
		return false;

	const std::string_view name = pSubsystem->Name();
	if (name.empty())
		return false;

	const bool isDuplicate = std::any_of(m_entries.begin(), m_entries.end(),
		[name](const SubsystemEntry& entry)
		{
			return entry.pSubsystem->Name() == name;
		});
	if (isDuplicate)
		return false;

	m_entries.push_back(SubsystemEntry{ std::move(pSubsystem), false });

	if (pFixedUpdatable)
		m_pFixedUpdatables.push_back(pFixedUpdatable);

	if (pUpdatable)
		m_pUpdatables.push_back(pUpdatable);

	if (pRenderable)
		m_pRenderables.push_back(pRenderable);

	return true;
}

/*---------------------------------------------------------------------------------------
| --- FixedUpdate: 
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineManager::FixedUpdate(float fixedDeltaTime)
{
	for (IFixedUpdatable* pFixedUpdatable : m_pFixedUpdatables)
	{
		pFixedUpdatable->FixedUpdate(fixedDeltaTime);
	}
}

/*---------------------------------------------------------------------------------------
| --- Update:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineManager::Update(float deltaTime)
{
	for (IUpdatable* pUpdatable : m_pUpdatables)
	{
		pUpdatable->Update(deltaTime);
	}
}

/*---------------------------------------------------------------------------------------
| --- Render:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineManager::Render()
{
	for (IRenderable* pRenderable : m_pRenderables)
	{
		pRenderable->BeginFrame();
	}

	for (IRenderable* pRenderable : m_pRenderables)
	{
		pRenderable->Render();
	}

	for (IRenderable* pRenderable : m_pRenderables)
	{
		pRenderable->EndFrame();
	}
}