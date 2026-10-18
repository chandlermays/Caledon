#include "LayerMask.h"
#include "LayerRegistry.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------------------------
| --- ToString:
---------------------------------------------------------------------------------------*/
std::string CaledonEngine::LayerMask::ToString(const LayerRegistry& registry) const
{
	return registry.MaskToString(*this);
}

/*---------------------------------------------------------------------------------------
| --- GetMask:
---------------------------------------------------------------------------------------*/
CaledonEngine::LayerMask CaledonEngine::LayerMask::GetMask(const LayerRegistry& registry, std::span<const std::string_view> layerNames)
{
	return registry.GetMask(layerNames);
}

/*---------------------------------------------------------------------------------------
| --- GetMask:
---------------------------------------------------------------------------------------*/
CaledonEngine::LayerMask CaledonEngine::LayerMask::GetMask(const LayerRegistry& registry, std::initializer_list<std::string_view> layerNames)
{
	return registry.GetMask(layerNames);
}

/*---------------------------------------------------------------------------------------
| --- LayerToName:
---------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::LayerMask::LayerToName(const LayerRegistry& registry, int layer)
{
	return registry.LayerToName(layer);
}

/*---------------------------------------------------------------------------------------
| --- NameToLayer:
---------------------------------------------------------------------------------------*/
int CaledonEngine::LayerMask::NameToLayer(const LayerRegistry& registry, std::string_view layerName)
{
	return registry.NameToLayer(layerName);
}