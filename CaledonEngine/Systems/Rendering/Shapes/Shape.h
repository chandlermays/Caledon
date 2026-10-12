/*------------------------------
| File: Shape.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Rendering/Color.h"
#include "Utilities/Math/Rect.h"

namespace CaledonEngine
{
	class Renderer;

	enum class ShapeType
	{
		kSquare,
		kCircle,
		kTriangle,
		kCapsule
	};

	class Shape
	{
	protected:
		ShapeType m_shapeType;																					// The type of the shape
		RectInt m_bounds;																						// The bounding rectangle of the shape
		Color m_color;																							// The color of the shape
		int m_width;																							// The width of the shape
		int m_height;																							// The height of the shape
		bool m_isFilled;																						// Whether the shape is filled or just an outline

	public:
		Shape(ShapeType type, const Color& color = Color::White(),
			int width = 100, int height = 100, bool isFilled = true);											// Constructor

		virtual ~Shape() = default;																				// Destructor
		Shape(const Shape&) = delete;																			// Prevent copy-construction
		Shape& operator=(const Shape&) = delete;																// Prevent copy-assignment
		Shape(Shape&&) = delete;																				// Prevent move-construction
		Shape& operator=(Shape&&) = delete;																		// Prevent move-assignment

		virtual void Render(Renderer* pRenderer, const RectFloat& destRect, const Color& color) const = 0;		// Draws the shape onto the screen

		void SetColor(const Color& color);																		// Sets the color of the shape
		void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);				// Sets the color of the shape
		void SetWidth(int width);																				// Sets the width of the shape
		void SetHeight(int height);																				// Sets the height of the shape
		void SetSize(int width, int height);																	// Sets the size of the shape
		void SetFilled(bool isFilled);																			// Sets whether the shape is filled or just an outline

		ShapeType GetShapeType() const;																			// Returns the type of the shape
		RectInt GetBounds() const;																				// Returns the bounding rectangle of the shape
		const Color& GetColor() const;																			// Returns the color of the shape
		int GetWidth() const;																					// Returns the width of the shape
		int GetHeight() const;																					// Returns the height of the shape
		bool IsFilled() const;																					// Returns whether the shape is filled or just an outline
	};
}