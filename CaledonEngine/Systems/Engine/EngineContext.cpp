/*------------------------------
| File: EngineContext.cpp
| Author: Chandler Mays
------------------------------*/
#include "EngineContext.h"

#include <algorithm>

/*------------------------------------------------------------------------------------
| --- Constructor: Constructs the EngineContext with the specified configuration --- |
------------------------------------------------------------------------------------*/
CaledonEngine::EngineContext::EngineContext(const EngineConfig& config)
	: m_config{ config }
	, m_published{}
	, m_quitRequested{ false }
{}

/*---------------------------------------------------------------------------------------
| --- FindSubsystem:
---------------------------------------------------------------------------------------*/
CaledonEngine::ISubsystem* CaledonEngine::EngineContext::FindSubsystem(std::string_view name) const noexcept
{
	for (const PublishedEntry& entry : m_published)
	{
		if (std::string_view{ entry.name } == name)
			return entry.pSubsystem;
	}

	return nullptr;
}

/*---------------------------------------------------------------------------------------
| --- Config:
---------------------------------------------------------------------------------------*/
const CaledonEngine::EngineConfig& CaledonEngine::EngineContext::Config() const noexcept
{
	return m_config;
}

/*---------------------------------------------------------------------------------------
| --- RequestQuit:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineContext::RequestQuit() noexcept
{
	m_quitRequested.store(true);
}

/*---------------------------------------------------------------------------------------
| --- IsQuitRequested:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::EngineContext::IsQuitRequested() const noexcept
{
	return m_quitRequested.load();
}

/*---------------------------------------------------------------------------------------
| --- Publish:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineContext::Publish(ISubsystem& subsystem)
{
	m_published.push_back(PublishedEntry{ std::string{ subsystem.Name() }, &subsystem });
}

/*---------------------------------------------------------------------------------------
| --- Retract:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineContext::Retract(ISubsystem& subsystem) noexcept
{
	std::erase_if(m_published, [&subsystem](const PublishedEntry& entry)
		{
			return entry.pSubsystem == &subsystem;
		});
}

/*---------------------------------------------------------------------------------------
| --- ResetQuit:
---------------------------------------------------------------------------------------*/
void CaledonEngine::EngineContext::ResetQuit() noexcept
{
	m_quitRequested.store(false);
}