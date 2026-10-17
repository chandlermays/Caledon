/*------------------------------
| File: Physics2DSubsystem.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Systems/Engine/ISubsystem.h"
#include "Systems/Engine/IFixedUpdatable.h"

#include <string_view>

// Provides utilities and global settings to manage and simulate 2D physics interactions

namespace CaledonEngine
{
	class Physics2DSubsystem : public ISubsystem, public IFixedUpdatable
	{
//	static properties:
//		static int AllLayers									// Layer mask constant that includes all layers.
//		static float angularSleepTolerance						// A Rigidbody cannot sleep if its angular velocity is above this tolerance threshold

	public:
		static constexpr std::string_view kName = "Physics2D";

		Physics2DSubsystem();
		~Physics2DSubsystem() override = default;
		Physics2DSubsystem(const Physics2DSubsystem&) = delete;
		Physics2DSubsystem& operator=(const Physics2DSubsystem&) = delete;
		Physics2DSubsystem(Physics2DSubsystem&&) = delete;
		Physics2DSubsystem& operator=(Physics2DSubsystem&&) = delete;

		std::string_view Name() const noexcept override;
		bool Initialize(EngineContext& context) override;
		void Shutdown() noexcept override;

		void FixedUpdate(float fixedDeltaTime) override;
	};
}