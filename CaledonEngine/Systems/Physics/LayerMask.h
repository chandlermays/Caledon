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
		int value = 0;																											// The bitmask value representing the layers included in this LayerMask

		constexpr LayerMask() noexcept = default;																				// Constructor

		constexpr LayerMask(int mask) noexcept																					// Parameterized Constructor
			: value{ mask }
		{ }

		constexpr operator int() const noexcept								{ return value; }									// Implicit conversion to an integer representing the bitmask value of the LayerMask

		std::string ToString(const LayerRegistry& registry) const;																// Lists the names of the included layers represented as a string

		static LayerMask GetMask(const LayerRegistry& registry, std::span<const std::string_view> layerNames);					// Returns a LayerMask that includes the layers specified by the given names
		static LayerMask GetMask(const LayerRegistry& registry, std::initializer_list<std::string_view> layerNames);			// Returns a LayerMask that includes the layers specified by the given names
		static std::string_view LayerToName(const LayerRegistry& registry, int layer);											// Returns the name of the layer corresponding to the given layer index
		static int NameToLayer(const LayerRegistry& registry, std::string_view layerName);										// Returns the index of the layer corresponding to the given layer name
	};
}