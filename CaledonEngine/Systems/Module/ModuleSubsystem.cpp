/*------------------------------
| File: ModuleSubsystem.cpp
| Author: Chandler Mays
------------------------------*/
#include "ModuleSubsystem.h"

#include "../Engine/EngineContext.h"
#include "../Engine/LoggingSubsystem.h"
#include "../Input/InputManager.h"
#include "../Module/ComponentRegistry.h"

#include <utility>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------------
| --- Constructor: Constructs the ModuleSubsystem with default values --- |
-------------------------------------------------------------------------*/
CaledonEngine::ModuleSubsystem::ModuleSubsystem(std::string modulePath)
    : m_path{ std::move(modulePath) }
    , m_host{ }
    , m_pUnregister{ nullptr }
    , m_pContext{ nullptr }
    , m_pRegistry{ nullptr }
    , m_pInput{ nullptr }
    , m_hasInputActions{ false }
{ }

/*---------------------------------------------------------------------------------------
| --- Name:
---------------------------------------------------------------------------------------*/
std::string_view CaledonEngine::ModuleSubsystem::Name() const noexcept
{
    return kName;
}

/*---------------------------------------------------------------------------------------
| --- Initialize:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::ModuleSubsystem::Initialize(EngineContext& context)
{
    m_pContext = &context;
    m_pRegistry = context.Get<ComponentRegistry>();
    m_pInput = context.Get<InputSubsystem>();

    if (!m_pRegistry || !m_pInput)
    {
        CE_LOG(context, LogLevel::Error, "Module", "Needs the ComponentRegistry and Input Subsystems.");
        Teardown();
        return false;
    }

    if (!m_library.Load(m_path))
    {
        CE_LOG(context, LogLevel::Error, "Module", "Could not load '" + m_path + "' (error " + std::to_string(m_library.GetLastError()) + ")");
        Teardown();
        return false;
    }

    auto pRegister = m_library.GetFunction<ModuleInterface::RegisterFunc>(ModuleInterface::kRegisterFunctionName);
    auto pUnregister = m_library.GetFunction<ModuleInterface::UnregisterFunc>(ModuleInterface::kUnregisterFunctionName);
    if (!pRegister || !pUnregister)
    {
        CE_LOG(context, LogLevel::Error, "Module", "'" + m_path + "' must export both '" + ModuleInterface::kRegisterFunctionName
            + "' and '" + ModuleInterface::kUnregisterFunctionName + "'");
        Teardown();
        return false;
    }

    m_host.version = ModuleInterface::kVersion;
    m_host.user = this;
    m_host.RegisterComponent = &ModuleSubsystem::HostRegisterComponent;
    m_host.SetInputActions = &ModuleSubsystem::HostSetInputActions;

    if (!pRegister(&m_host))
    {
        CE_LOG(context, LogLevel::Error, "Module", "'" + m_path + "' failed to register");
        Teardown();
        return false;
    }

    m_pUnregister = pUnregister;
    CE_LOG(context, LogLevel::Display, "Module", "Loaded '" + m_path + "' (" + std::to_string(m_registeredComponents.size()) + " components)");
    return true;
}

/*---------------------------------------------------------------------------------------
| --- Shutdown:
---------------------------------------------------------------------------------------*/
void CaledonEngine::ModuleSubsystem::Shutdown() noexcept
{
    Teardown();
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*---------------------------------------------------------------------------------------
| --- HostRegisterComponent:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::ModuleSubsystem::HostRegisterComponent(void* user, const char* name, Component* (*create)())
{
    auto* self = static_cast<ModuleSubsystem*>(user);
    if (!self || !self->m_pRegistry || !name)
        return false;

    if (!self->m_pRegistry->Register(name, create))
        return false;

    self->m_registeredComponents.emplace_back(name);

    return true;
}

/*---------------------------------------------------------------------------------------
| --- HostSetInputActions:
---------------------------------------------------------------------------------------*/
void CaledonEngine::ModuleSubsystem::HostSetInputActions(void* user, InputActions* pActions)
{
    auto* self = static_cast<ModuleSubsystem*>(user);
    if (!self || !self->m_pInput)
        return;

    self->m_pInput->SetInputActions(pActions);
    self->m_hasInputActions = (pActions != nullptr);
}

/*---------------------------------------------------------------------------------------
| --- Teardown:
---------------------------------------------------------------------------------------*/
void CaledonEngine::ModuleSubsystem::Teardown() noexcept
{
    // Unregister the container of components
    if (m_pRegistry)
    {
        for (const std::string& name : m_registeredComponents)
        {
            m_pRegistry->Unregister(name);
        }
    }
    m_registeredComponents.clear();

    // Unregister the input actions
    if (m_hasInputActions && m_pInput)
    {
        m_pInput->SetInputActions(nullptr);
    }
    m_hasInputActions = false;

    // Unregister the Host
    if (m_pUnregister)
    {
        m_pUnregister(&m_host);
    }
    m_pUnregister = nullptr;

    // Unload the Library
    m_library.Unload();

    // Clean up the references
    m_pRegistry = nullptr;
    m_pInput = nullptr;
    m_pContext = nullptr;
}