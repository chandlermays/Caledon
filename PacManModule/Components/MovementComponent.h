/*------------------------------
| File: MovementComponent.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "CaledonEngine/Systems/Scene/Component.h"
#include "CaledonEngine/Utilities/Math/Vector2.h"

namespace PacmanModule
{
	class MovementComponent : public CaledonEngine::Component
	{
	private:
		CaledonEngine::Vector2f m_direction;														//
		float m_speed;																				//

	public:
		MovementComponent();																		// Constructor
		~MovementComponent() override = default;													// Destructor
		MovementComponent(const MovementComponent&) = delete;										// Prevent copy-construction
		MovementComponent& operator=(const MovementComponent&) = delete;							// Prevent copy-assignment
		MovementComponent(MovementComponent&&) = delete;											// Prevent move-construction
		MovementComponent& operator=(MovementComponent&&) = delete;									// Prevent move-assignment

		const std::string& GetTypeName() const override;											//

		void Update(float deltaTime) override;														//

		void SetDirection(const CaledonEngine::Vector2f direction);									//
		const CaledonEngine::Vector2f& GetDirection() const;										//
		
		void SetSpeed(float speed);																	//
		float GetSpeed() const;																		//

		void Stop();																				//
	};
}