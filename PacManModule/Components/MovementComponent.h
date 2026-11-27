/*------------------------------
| File: MovementComponent.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "CaledonEngine/Systems/Scene/Component.h"
#include "CaledonEngine/Utilities/Math/Vector2.h"

namespace PacmanModule
{
	/*----------------------------------------------------------------------------------
	| --- MovementComponent: Moves its GameObject in a direction at a speed. With  --- |
	| ---                    a RigidBody2D it sets the body's velocity, so physics --- |
	| ---                    moves it and stops it at colliders; without one it    --- |
	| ---                    moves the Transform directly                          --- |
	----------------------------------------------------------------------------------*/
	class MovementComponent : public CaledonEngine::Component
	{
	private:
		CaledonEngine::Vector2f m_direction;														// The direction to move in
		float m_speed;																				// The speed to move at, in units per second

	public:
		MovementComponent();																		// Constructor
		~MovementComponent() override = default;													// Destructor
		MovementComponent(const MovementComponent&) = delete;										// Prevent copy-construction
		MovementComponent& operator=(const MovementComponent&) = delete;							// Prevent copy-assignment
		MovementComponent(MovementComponent&&) = delete;											// Prevent move-construction
		MovementComponent& operator=(MovementComponent&&) = delete;									// Prevent move-assignment

		const std::string& GetTypeName() const override;											// Returns the type name of the Component

		void Update(float deltaTime) override;														// Moves the owner (through its RigidBody2D when it has one)

		void SetDirection(const CaledonEngine::Vector2f direction);									// Sets the direction to move in
		const CaledonEngine::Vector2f& GetDirection() const;										// Returns the direction to move in

		void SetSpeed(float speed);																	// Sets the speed to move at
		float GetSpeed() const;																		// Returns the speed to move at

		void Stop();																				// Stops moving by clearing the direction
	};
}