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
	, m_published{ }
	, m_quitRequested{ false }
{ }

/*----------------------------------------------------------------------------------------
| --- FindSubsystem: Returns the published subsystem with the given name, or nullptr --- |
----------------------------------------------------------------------------------------*/
CaledonEngine::ISubsystem* CaledonEngine::EngineContext::FindSubsystem(std::string_view name) const noexcept
{
	for (const PublishedEntry& entry : m_published)
	{
		if (std::string_view{ entry.name } == name)
			return entry.pSubsystem;
	}

	return nullptr;
}

/*--------------------------------------------------
| --- Config: Returns the engine configuration --- |
--------------------------------------------------*/
const CaledonEngine::EngineConfig& CaledonEngine::EngineContext::Config() const noexcept
{
	return m_config;
}

/*-------------------------------------------------------------------------
| --- RequestQuit: Asks the main loop to stop at the end of the frame --- |
-------------------------------------------------------------------------*/
void CaledonEngine::EngineContext::RequestQuit() noexcept
{
	m_quitRequested.store(true);
}

/*--------------------------------------------------------------------
| --- IsQuitRequested: Returns whether a quit has been requested --- |
--------------------------------------------------------------------*/
bool CaledonEngine::EngineContext::IsQuitRequested() const noexcept
{
	return m_quitRequested.load();
}

/*----------------------------------------------------------------------
| --- Publish: Makes an initialized subsystem available for lookup --- |
----------------------------------------------------------------------*/
void CaledonEngine::EngineContext::Publish(ISubsystem& subsystem)
{
	m_published.push_back(PublishedEntry{ std::string{ subsystem.Name() }, &subsystem });
}

/*--------------------------------------------------
| --- Retract: Removes a subsystem from lookup --- |
--------------------------------------------------*/
void CaledonEngine::EngineContext::Retract(ISubsystem& subsystem) noexcept
{
	std::erase_if(m_published, [&subsystem](const PublishedEntry& entry)
		{
			return entry.pSubsystem == &subsystem;
		});
}

/*--------------------------------------------
| --- ResetQuit: Clears the quit request --- |
--------------------------------------------*/
void CaledonEngine::EngineContext::ResetQuit() noexcept
{
	m_quitRequested.store(false);
}