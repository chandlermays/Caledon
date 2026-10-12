/*------------------------------
| File: Transform.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Scene/Component.h"
#include "Utilities/Math/Vector2.h"

namespace CaledonEngine
{
	class Transform : public Component
	{
	private:
		Vector2f m_position;																		// The position of the object
		float m_rotation;																			// The rotation of the object
		Vector2f m_scale;																			// The scale of the object

	public:
		Transform();																				// Constructor
		Transform(const Vector2f& position, float rotation, const Vector2f& scale);					// Parameterized Constructor
		~Transform() override = default;															// Destructor
		Transform(const Transform&) = delete;														// Prevent copy-construction
		Transform& operator=(const Transform&) = delete;											// Prevent copy-assignment
		Transform(Transform&&) = delete;															// Prevent move-construction
		Transform& operator=(Transform&&) = delete;													// Prevent move-assignment

		const std::string& GetTypeName() const override;											// Returns the type name of the Component

		const Vector2f& GetPosition() const;														// Returns the position of the object
		void SetPosition(const Vector2f& position);													// Sets the position of the object

		float GetRotation() const;																	// Returns the rotation of the object
		void SetRotation(float rotation);															// Sets the rotation of the object

		const Vector2f& GetScale() const;															// Returns the scale of the object
		void SetScale(const Vector2f& scale);														// Sets the scale of the object
	};
}