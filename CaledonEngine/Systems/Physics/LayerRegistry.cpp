/*------------------------------
| File: LayerRegistry.cpp
| Author: Chandler Mays
------------------------------*/
#include "LayerRegistry.h"
#include "../Engine/EngineContext.h"
#include "../Engine/LoggingSubsystem.h"

namespace
{
	struct BuiltInLayer { int index; const char* name; };

	constexpr BuiltInLayer kBuiltInLayers[] =
	{
		{ 0, "Default" },
		{ 1, "TransparentFX" },
		{ 2, "Ignore Raycast" },
		{ 3, "Water" },
		{ 4, "UI" },
	};
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-----------------------------------------------------------------------
| --- Constructor: Constructs the LayerRegistry with default values --- |
-----------------------------------------------------------------------*/
CaledonEngine::LayerRegistry::LayerRegistry()
	: m_pContext{ nullptr }
{
}

/*---------------------------------------------------------------------------------------
| --- Name:
---------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::LayerRegistry::Name() const noexcept
{
	return kName;
}

/*---------------------------------------------------------------------------------------
| --- Initialize: Installs the BuiltIn layers --- |
---------------------------------------------------------------------------------------*/
bool CaledonEngine::LayerRegistry::Initialize(EngineContext& context)
{
	m_pContext = &context;

	for (auto& name : m_names)
		name.clear();

	for (const auto& builtIn : kBuiltInLayers)
		m_names[builtIn.index] = builtIn.name;

	return true;
}

/*---------------------------------------------------------------------------------------
| --- Shutdown:
---------------------------------------------------------------------------------------*/
void CaledonEngine::LayerRegistry::Shutdown() noexcept
{
	for (auto& name : m_names)
		name.clear();

	m_pContext = nullptr;
}

/*---------------------------------------------------------------------------------------
| --- NameToLayer:
---------------------------------------------------------------------------------------*/
int CaledonEngine::LayerRegistry::NameToLayer(std::string_view layerName) const noexcept
{
	if (layerName.empty())
		return -1;

	for (int i = 0; i < kLayerCount; ++i)
	{
		if (m_names[i] == layerName)
			return i;
	}

	return -1;
}

/*---------------------------------------------------------------------------------------
| --- LayerToName:
---------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::LayerRegistry::LayerToName(int layer) const noexcept
{
	if (layer < 0 || layer >= kLayerCount)
		return {};

	return m_names[layer];
}

/*---------------------------------------------------------------------------------------
| --- IsDefined:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::LayerRegistry::IsDefined(int layer) const noexcept
{
	return layer >= 0 && layer < kLayerCount && !m_names[layer].empty();
}

/*---------------------------------------------------------------------------------------
| --- IsBuiltIn:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::LayerRegistry::IsBuiltIn(int layer) const noexcept
{
	return layer >= 0 && layer < kFirstUserLayer;
}

/*---------------------------------------------------------------------------------------
| --- GetMask:
---------------------------------------------------------------------------------------*/
CaledonEngine::LayerMask CaledonEngine::LayerRegistry::GetMask(std::span<const std::string_view> layerNames) const noexcept
{
	unsigned int mask = 0;

	for (std::string_view name : layerNames)
	{
		const int layer = NameToLayer(name);
		if (layer < 0)
		{
			if (m_pContext)
				CE_LOG(*m_pContext, LogLevel::Warning, "Layers", "GetMask: unknown layer '{}'.", name);
			continue;
		}

		mask |= (1u << layer);
	}

	return LayerMask{ static_cast<int>(mask) };
}

/*---------------------------------------------------------------------------------------
| --- GetMask:
---------------------------------------------------------------------------------------*/
CaledonEngine::LayerMask CaledonEngine::LayerRegistry::GetMask(std::initializer_list<std::string_view> layerNames) const noexcept
{
	return GetMask(std::span<const std::string_view>{ layerNames.begin(), layerNames.size() });
}

/*---------------------------------------------------------------------------------------
| --- MaskToString:
---------------------------------------------------------------------------------------*/
std::string CaledonEngine::LayerRegistry::MaskToString(LayerMask mask) const
{
	const unsigned int bits = static_cast<unsigned int>(mask.value);
	std::string result;

	for (int i = 0; i < kLayerCount; ++i)
	{
		if (!(bits & (1u << i)) || m_names[i].empty())
			continue;

		if (!result.empty())
			result += ", ";
		result += m_names[i];
	}

	return result;
}

/*---------------------------------------------------------------------------------------
| --- SetLayerName:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::LayerRegistry::SetLayerName(int layer, std::string_view layerName)
{
	if (layer < kFirstUserLayer || layer >= kLayerCount)
		return false;

	if (layerName.empty())
	{
		m_names[layer].clear();
		return true;
	}

	const int existing = NameToLayer(layerName);
	if (existing >= 0 && existing != layer)
		return false;

	m_names[layer] = layerName;
	return true;
}