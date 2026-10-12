/*------------------------------
| File: SDLTexture.cpp
| Author: Chandler Mays
------------------------------*/
#include "SDLTexture.h"

#include <SDL3/SDL.h>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-----------------------------------------------------------------------
| --- Constructor: Constructs the 'SDL' Texture with default values --- |
-----------------------------------------------------------------------*/
CaledonEngine::SDLTexture::SDLTexture()
    : m_pTexture{ nullptr }
    , m_width{ 0 }
    , m_height{ 0 }
{
}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::SDLTexture::~SDLTexture()
{
    if (m_pTexture)
    {
        SDL_DestroyTexture(m_pTexture);
        m_pTexture = nullptr;
    }
}

/*-------------------------------------------------------------
| --- Load: Loads the SDL texture and sets its dimensions --- |
-------------------------------------------------------------*/
bool CaledonEngine::SDLTexture::Load(SDL_Texture* pTexture, int width, int height)
{
    if (!pTexture)
        return false;

    if (m_pTexture && m_pTexture != pTexture)
    {
        SDL_DestroyTexture(m_pTexture);
    }

    m_pTexture = pTexture;
    m_width = width;
    m_height = height;
    return true;
}

/*----------------------------------------------------
| --- GetWidth: Returns the width of the texture --- |
----------------------------------------------------*/
int CaledonEngine::SDLTexture::GetWidth() const
{
    return m_width;
}

/*-----------------------------------------------------
| --- GeHeight: Returns the height of the texture --- |
-----------------------------------------------------*/
int CaledonEngine::SDLTexture::GetHeight() const
{
    return m_height;
}

/*------------------------------------------------------------
| --- GetNativeHandle: Returns the native texture handle --- |
------------------------------------------------------------*/
void* CaledonEngine::SDLTexture::GetNativeHandle() const
{
    return static_cast<void*>(m_pTexture);
}