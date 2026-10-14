/*------------------------------
| File: DynamicLibrary.cpp
| Author: Chandler Mays
------------------------------*/
#include "DynamicLibrary.h"

#include <Windows.h>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------
| --- Constructor: Constructs the DynamicLibrary with default values --- |
------------------------------------------------------------------------*/
CaledonEngine::DynamicLibrary::DynamicLibrary()
    : m_pHandle{ nullptr }
    , m_lastError{ 0 }
{}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::DynamicLibrary::~DynamicLibrary()
{
    Unload();
}

/*--------------------------------------------------------------------------
| --- Load:
--------------------------------------------------------------------------*/
bool CaledonEngine::DynamicLibrary::Load(const std::string& path)
{
    Unload();
    m_lastError = 0;

    m_pHandle = static_cast<void*>(LoadLibraryA(path.c_str()));
    if (!m_pHandle)
    {
        m_lastError = ::GetLastError();
    }
    return m_pHandle != nullptr;
}

/*--------------------------------------------------------------------------
| --- Unload:
--------------------------------------------------------------------------*/
void CaledonEngine::DynamicLibrary::Unload()
{
    if (m_pHandle)
    {
        FreeLibrary(static_cast<HMODULE>(m_pHandle));
        m_pHandle = nullptr;
    }
}

/*--------------------------------------------------------------------------
| --- IsLoaded:
--------------------------------------------------------------------------*/
bool CaledonEngine::DynamicLibrary::IsLoaded() const
{
    return m_pHandle != nullptr;
}

/*--------------------------------------------------------------------------
| --- GetLastError:
--------------------------------------------------------------------------*/
unsigned long CaledonEngine::DynamicLibrary::GetLastError() const
{
    return m_lastError;
}

/*--------------------------------------------------------------------------
| --- GetFunctionAddress:
--------------------------------------------------------------------------*/
void* CaledonEngine::DynamicLibrary::GetFunctionAddress(const std::string& functionName) const
{
    if (!m_pHandle)
        return nullptr;

    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(m_pHandle), functionName.c_str()));
}