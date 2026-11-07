#include "LayerMask.h"
#include "LayerRegistry.h"

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------------------
| --- ToString: Lists the names of the included layers represented as a string --- |
----------------------------------------------------------------------------------*/
std::string CaledonEngine::LayerMask::ToString(const LayerRegistry& registry) const
{
	return registry.MaskToString(*this);
}

/*--------------------------------------------------------------------------------------------
| --- GetMask: Returns a LayerMask that includes the layers specified by the given names --- |
--------------------------------------------------------------------------------------------*/
CaledonEngine::LayerMask CaledonEngine::LayerMask::GetMask(const LayerRegistry& registry, std::span<const std::string_view> layerNames)
{
	return registry.GetMask(layerNames);
}

/*--------------------------------------------------------------------------------------------
| --- GetMask: Returns a LayerMask that includes the layers specified by the given names --- |
--------------------------------------------------------------------------------------------*/
CaledonEngine::LayerMask CaledonEngine::LayerMask::GetMask(const LayerRegistry& registry, std::initializer_list<std::string_view> layerNames)
{
	return registry.GetMask(layerNames);
}

/*-------------------------------------------------------------------------------------------
| --- LayerToName: Returns the name of the layer corresponding to the given layer index --- |
-------------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::LayerMask::LayerToName(const LayerRegistry& registry, int layer)
{
	return registry.LayerToName(layer);
}

/*-------------------------------------------------------------------------------------------
| --- NameToLayer: Returns the index of the layer corresponding to the given layer name --- |
-------------------------------------------------------------------------------------------*/
int CaledonEngine::LayerMask::NameToLayer(const LayerRegistry& registry, std::string_view layerName)
{
	return registry.NameToLayer(layerName);
}