/*------------------------------
| File: Sprite.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "Utilities/Geometry/Rect.h"
#include "Utilities/Math/Vector2.h"

#include <memory>

namespace CaledonEngine
{
	class Texture;
	class Shape;

	enum class SpriteType
	{
		kTexture,
		kPrimitive
	};

	class Sprite
	{
	private:
		SpriteType m_type;																					// The type of the sprite

		std::shared_ptr<Texture> m_pTexture;																// The texture of the sprite
		RectInt m_textureRect;																				// The rectangle defining the texture area
		Vector2f m_pivot;																					// The pivot point of the sprite

		std::unique_ptr<Shape> m_pShape;																	// The shape of the sprite
		int m_pixelsPerUnit;																				// The number of pixels per unit for scaling
		Vector2f m_size;																					// The size of the sprite in world units

	public:
		Sprite();																							// Constructor
		~Sprite();																							// Destructor
		Sprite(const Sprite&) = delete;																		// Prevent copy-construction
		Sprite& operator=(const Sprite&) = delete;															// Prevent copy-assignment
		Sprite(Sprite&&) = delete;																			// Prevent move-construction
		Sprite& operator=(Sprite&&) = delete;																// Prevent move-assignment

		static std::unique_ptr<Sprite> CreateFromTexture(std::shared_ptr<Texture> pTexture,
			const RectInt& textureRect = RectInt(), const Vector2f& pivot = Vector2f(0.5f, 0.5f),
			int pixelsPerUnit = 100);																		// Creates a sprite from a texture

		static std::unique_ptr<Sprite> CreateFromShape(std::unique_ptr<Shape> pShape,
			int pixelsPerUnit = 100);																		// Creates a sprite from a shape

		void SetPivot(const Vector2f& pivot);																// Sets the pivot point of the sprite
		void SetPixelsPerUnit(int pixelsPerUnit);															// Sets the number of pixels per unit for scaling

		SpriteType GetType() const;																			// Returns the type of the sprite
		Texture* GetTexture() const;																		// Returns the texture of the sprite
		const RectInt& GetTextureRect() const;																// Returns the rectangle defining the texture area
		Vector2f GetTextureRectOffset() const;																// Returns the offset of the texture rectangle
		const Vector2f& GetPivot() const;																	// Returns the pivot point of the sprite
		Shape* GetShape() const;																			// Returns the shape of the sprite
		int GetPixelsPerUnit() const;																		// Returns the number of pixels per unit for scaling
		const Vector2f& GetSize() const;																	// Returns the size of the sprite in world units

		int GetPixelWidth() const;																			// Returns the width of the sprite in pixels
		int GetPixelHeight() const;																			// Returns the height of the sprite in pixels
	};
}