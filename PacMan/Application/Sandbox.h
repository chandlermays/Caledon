/*------------------------------
| File: SceneSandbox.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "CaledonEngine/Systems/Engine/ISubsystem.h"

#include <string_view>

namespace CaledonEngine
{
	class EngineContext;
}

namespace Game
{
	class Sandbox final : public CaledonEngine::ISubsystem
	{
	public:
		static constexpr std::string_view kName = "SceneSandbox";				//

		std::string_view Name() const noexcept override;						//
		bool Initialize(CaledonEngine::EngineContext& context) override;		//
		void Shutdown() noexcept override;										//
	};
}