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
		static constexpr std::string_view kName = "LayerRegistry";
		static constexpr int kLayerCount = 32;																		// 
		static constexpr int kFirstUserLayer = 8;																	// 

	private:
		std::array<std::string, kLayerCount> m_names;																// 
		EngineContext* m_pContext;																					// 

	public:
		LayerRegistry();																							// Constructor
		~LayerRegistry() override = default;																		// Destructor
		LayerRegistry(const LayerRegistry&) = delete;																// Prevent copy-construction
		LayerRegistry& operator=(const LayerRegistry&) = delete;													// Prevent copy-assignment
		LayerRegistry(LayerRegistry&&) = delete;																	// Prevent move-construction
		LayerRegistry& operator=(LayerRegistry&&) = delete;															// Prevent move-assignment

		std::string_view Name() const noexcept override;															//
		bool Initialize(EngineContext& context) override;															//
		void Shutdown() noexcept override;																			//

		int NameToLayer(std::string_view layerName) const noexcept;													//
		std::string_view LayerToName(int layer) const noexcept;														// 
		bool IsDefined(int layer) const noexcept;																	// 
		bool IsBuiltIn(int layer) const noexcept;																	// 
		
		LayerMask GetMask(std::span<const std::string_view> layerNames) const noexcept;								// 
		LayerMask GetMask(std::initializer_list<std::string_view> layerNames) const noexcept;						//
		std::string MaskToString(LayerMask mask) const;																//

		bool SetLayerName(int layer, std::string_view layerName);													//
	};
}