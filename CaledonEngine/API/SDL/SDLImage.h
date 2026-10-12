/*------------------------------
| File: SDLImage.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Rendering/Resources/Image.h"

#include <memory>

struct SDL_Surface;

namespace CaledonEngine
{
    class SDLImage : public Image
    {
    private:
        SDL_Surface* m_pSurface;                                                                // The SDL Surface (Image)

    public:
        SDLImage();                                                                             // Constructor
        explicit SDLImage(SDL_Surface* pSurface);                                               // Parameterized Constructor
        ~SDLImage() override;                                                                   // Destructor
        SDLImage(const SDLImage&) = delete;                                                     // Prevent copy-construction
        SDLImage& operator=(const SDLImage&) = delete;                                          // Prevent copy-assignment
        SDLImage(SDLImage&&) = delete;                                                          // Prevent move-construction
        SDLImage& operator=(SDLImage&&) = delete;                                               // Prevent move-assignment

        int GetW() const override;                                                              // Get the width of the image
        int GetH() const override;											                    // Get the height of the image

        static std::unique_ptr<SDLImage> CreateImage(const std::string& filePath);	            // Create an Image from a File

        void* GetNativeHandle() const override;                                                 // Get the native image handle (SDL_Surface*)
    };
}