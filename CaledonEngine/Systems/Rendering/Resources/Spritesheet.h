/*------------------------------
| File: SpriteSheet.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Utilities/Math/Rect.h"
#include "Utilities/Math/Vector2.h"

#include <memory>

namespace CaledonEngine
{
	class Texture;
	class Sprite;

	class SpriteSheet
	{
	private:
		std::shared_ptr<Texture> m_pTexture;
		int m_frameWidth;
		int m_frameHeight;
		int m_columns;

	public:
		SpriteSheet(std::shared_ptr<Texture> pTexture, int frameWidth, int frameHeight);									// Constructor
		~SpriteSheet() = default;																							// Destructor
		SpriteSheet(const SpriteSheet&) = delete;																			// Prevent copy-construction
		SpriteSheet& operator=(const SpriteSheet&) = delete;																// Prevent copy-assignment
		SpriteSheet(SpriteSheet&&) = delete;																				// Prevent move-construction
		SpriteSheet& operator=(SpriteSheet&&) = delete;																		// Prevent move-assignment

		RectInt GetFrameRect(int index) const;																				// Returns the rectangle of the specified frame index
		std::unique_ptr<Sprite> CreateSprite(int index, const Vector2f& pivot = Vector2f(0.5f, 0.5f),
			int pixelsPerUnit = 100) const;																					// Creates a sprite from the specified frame index

		Texture* GetTexture() const;																						// Returns a pointer to the texture
		int GetFrameWidth() const;																							// Returns the width of a single frame
		int GetFrameHeight() const;																							// Returns the height of a single frame
		int GetFrameCount() const;																							// Returns the total number of frames in the spritesheet
	};
}