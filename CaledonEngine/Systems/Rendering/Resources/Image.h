/*------------------------------
| File: Image.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <string>

namespace CaledonEngine
{
	class Image
	{
	public:
		virtual ~Image() = default;							// Destructor

		virtual int GetW() const = 0;						// Get the width of the image
		virtual int GetH() const = 0;						// Get the height of the image
		virtual void* GetNativeHandle() const = 0;			// Get the native image handle (platform-specific)
	};
}