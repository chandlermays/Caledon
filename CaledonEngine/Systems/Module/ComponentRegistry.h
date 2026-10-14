/*------------------------------
| File: ComponentRegistry.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "../Engine/ISubsystem.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace CaledonEngine
{
	class Component;

	class ComponentRegistry : public ISubsystem
	{
	public:
		using CreateFunction = Component * (*)();

	private:
		std::unordered_map<std::string, CreateFunction> m_creators;

	public:
		static constexpr std::string_view kName = "ComponentRegistry";

		ComponentRegistry() = default;														// Constructor
		~ComponentRegistry() override = default;											// Destructor
		ComponentRegistry(const ComponentRegistry&) = delete;								// Prevent copy-construction
		ComponentRegistry& operator=(const ComponentRegistry&) = delete;					// Prevent copy-assignment
		ComponentRegistry(ComponentRegistry&&) = delete;									// Prevent move-construction
		ComponentRegistry& operator=(ComponentRegistry&&) = delete;							// Prevent move-assignment

		std::string_view Name() const noexcept override;
		bool Initialize(EngineContext& context) override;
		void Shutdown() noexcept override;

		bool Register(std::string_view name, CreateFunction create);
		bool Unregister(std::string_view name) noexcept;
		bool IsRegistered(std::string_view name) const;
		std::unique_ptr<Component> Create(std::string_view name) const;
	};
}