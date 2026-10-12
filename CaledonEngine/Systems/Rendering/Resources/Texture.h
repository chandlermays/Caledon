/*------------------------------
| File: Texture.h
| Author: Chandler Mays
------------------------------*/
#pragma once

namespace CaledonEngine
{
	class Texture
	{
	public:
		virtual ~Texture() = default;						// Destructor

		virtual int GetWidth() const = 0;					// Returns the width of the texture
		virtual int GetHeight() const = 0;					// Returns the height of the texture

		virtual void* GetNativeHandle() const = 0;			// Returns the native texture handle
	};
}