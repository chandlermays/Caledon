/*------------------------------
| File: LayerRegistry.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Engine/ISubsystem.h"
#include "Systems/Physics/LayerMask.h"

#include <array>
#include <initializer_list>
#include <span>
#include <string>
#include <string_view>

namespace CaledonEngine
{
	class EngineContext;

	class LayerRegistry final : public ISubsystem
	{
	public:
		static constexpr std::string_view kName = "LayerRegistry";													// The published name of this subsystem in EngineContext
		static constexpr int kLayerCount = 32;																		// The total number of layers available in the registry
		static constexpr int kFirstUserLayer = 8;																	// The index of the first user-defined layer

	private:
		std::array<std::string, kLayerCount> m_names;																// The names of the layers, indexed by layer number
		EngineContext* m_pContext;																					// The engine context that owns this subsystem

	public:
		LayerRegistry();																							// Constructor
		~LayerRegistry() override = default;																		// Destructor
		LayerRegistry(const LayerRegistry&) = delete;																// Prevent copy-construction
		LayerRegistry& operator=(const LayerRegistry&) = delete;													// Prevent copy-assignment
		LayerRegistry(LayerRegistry&&) = delete;																	// Prevent move-construction
		LayerRegistry& operator=(LayerRegistry&&) = delete;															// Prevent move-assignment

		std::string_view Name() const noexcept override;															// Returns the name of the subsystem
		bool Initialize(EngineContext& context) override;															// Prepares the LayerRegistry for use
		void Shutdown() noexcept override;																			// Cleans up the LayerRegistry and releases any resources

		int NameToLayer(std::string_view layerName) const noexcept;													// Returns the index of the layer corresponding to the given layer name
		std::string_view LayerToName(int layer) const noexcept;														// Returns the name of the layer corresponding to the given layer index
		bool IsDefined(int layer) const noexcept;																	// Returns true if the layer has a name defined in the registry
		bool IsBuiltIn(int layer) const noexcept;																	// Returns true if the layer is a built-in layer (0-7) and cannot be changed by the user
		
		LayerMask GetMask(std::span<const std::string_view> layerNames) const noexcept;								// Returns a LayerMask that includes the layers specified by the given names
		LayerMask GetMask(std::initializer_list<std::string_view> layerNames) const noexcept;						// Returns a LayerMask that includes the layers specified by the given names
		std::string MaskToString(LayerMask mask) const;																// Returns a string representation of the layers included in the given LayerMask

		bool SetLayerName(int layer, std::string_view layerName);													// Sets the name of the layer at the specified index
	};
}