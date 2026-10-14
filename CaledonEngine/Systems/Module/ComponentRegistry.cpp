/*------------------------------
| File: ComponentRegistry.cpp
| Author: Chandler Mays
------------------------------*/
#include "ComponentRegistry.h"

#include "../Scene/Component.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*--------------------------------------------------------------------------
| --- Name: 
--------------------------------------------------------------------------*/
std::string_view CaledonEngine::ComponentRegistry::Name() const noexcept
{
	return kName;
}

/*--------------------------------------------------------------------------
| --- Initialize:
--------------------------------------------------------------------------*/
bool CaledonEngine::ComponentRegistry::Initialize(EngineContext& context)
{
	//...
	return true;
}

/*--------------------------------------------------------------------------
| --- Shutdown:
--------------------------------------------------------------------------*/
void CaledonEngine::ComponentRegistry::Shutdown() noexcept
{
	m_creators.clear();
}

/*--------------------------------------------------------------------------
| --- Register:
--------------------------------------------------------------------------*/
bool CaledonEngine::ComponentRegistry::Register(std::string_view name, CreateFunction create)
{
	if (name.empty() || !create)
		return false;

	return m_creators.emplace(std::string{ name }, create).second;
}

/*--------------------------------------------------------------------------
| --- Unregister:
--------------------------------------------------------------------------*/
bool CaledonEngine::ComponentRegistry::Unregister(std::string_view name) noexcept
{
	return m_creators.erase(std::string{ name }) > 0;
}

/*--------------------------------------------------------------------------
| --- IsRegistered:
--------------------------------------------------------------------------*/
bool CaledonEngine::ComponentRegistry::IsRegistered(std::string_view name) const
{
	return m_creators.find(std::string{ name }) != m_creators.end();
}

/*--------------------------------------------------------------------------
| --- Create:
--------------------------------------------------------------------------*/
std::unique_ptr<CaledonEngine::Component> CaledonEngine::ComponentRegistry::Create(std::string_view name) const
{
	auto it = m_creators.find(std::string{ name });
	if (it == m_creators.end())
		return nullptr;

	return std::unique_ptr<CaledonEngine::Component>{ it->second() };
}