/*------------------------------
| File: SpriteComponent.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Scene/Component.h"
#include "Systems/Rendering/Color.h"

#include <cstdint>
#include <memory>
#include <string>

namespace CaledonEngine
{
	class Sprite;

	class SpriteComponent : public Component
	{
	private:
		std::unique_ptr<Sprite> m_pSprite;															// Pointer to the Sprite asset
		Color m_color;																				// Color of the sprite
		int m_layer;																				// Draw order layer of the sprite

	public:
		SpriteComponent();																			// Constructor
		~SpriteComponent();																			// Destructor
		SpriteComponent(const SpriteComponent&) = delete;											// Prevent copy-construction
		SpriteComponent& operator=(const SpriteComponent&) = delete;								// Prevent copy-assignment
		SpriteComponent(SpriteComponent&&) = delete;												// Prevent move-construction
		SpriteComponent& operator=(SpriteComponent&&) = delete;										// Prevent move-assignment

		void SetSprite(std::unique_ptr<Sprite> pSprite);											// Sets the sprite to be rendered
		void SetColor(const Color& color);															// Sets the color of the sprite
		void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);									// Sets the color of the sprite
		void SetLayer(int layer);																	// Sets the draw order layer of the sprite

		const std::string& GetTypeName() const override;											// Returns the type name of the Component
		Sprite* GetSprite() const;																	// Returns a pointer to the sprite
		const Color& GetColor() const;																// Returns the color of the sprite
		int GetLayer() const;																		// Returns the draw order layer
	};
}