/*------------------------------
| File: LayerMask.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <initializer_list>
#include <span>
#include <string>
#include <string_view>

// Specifies Layers to use in a Physics.Raycast
// The names are as defined in either a BuiltIn or a User Layer in the LayerRegistry
//
// A LayerMask is only a bit set: bit N set = Layer N is included. It holds no names. Names live in the
// LayerRegistry subsystem (reached through EngineContext), so every name lookup below takes the registry
// explicitly instead of reaching for a static, which would be a different object in the Game and GameModule.

namespace CaledonEngine
{
	class LayerRegistry;

	struct LayerMask
	{
		int value = 0;																											// 

		constexpr LayerMask() noexcept = default;																				// 

		constexpr LayerMask(int mask) noexcept																					//
			: value{ mask }
		{ }

		constexpr operator int() const noexcept { return value; }																// 

		std::string ToString(const LayerRegistry& registry) const;																// 

		static LayerMask GetMask(const LayerRegistry& registry, std::span<const std::string_view> layerNames);					// 
		static LayerMask GetMask(const LayerRegistry& registry, std::initializer_list<std::string_view> layerNames);			// 
		static std::string_view LayerToName(const LayerRegistry& registry, int layer);											// 
		static int NameToLayer(const LayerRegistry& registry, std::string_view layerName);										//
	};
}