/*------------------------------
| File: BodyIntegration2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "BodyIntegration2D.h"

#include "Utilities/Math/Constants.h"

#include <cmath>

/*-------------------------------------------------------------------------------------------------
| --- ApplyDamping: Applies an exponential-like velocity decay based on damping and deltaTime --- |
-------------------------------------------------------------------------------------------------*/
float CaledonEngine::BodyIntegration2D::ApplyDamping(float velocity, float damping, float deltaTime) noexcept
{
	return velocity / (1.0f + deltaTime * std::fmax(damping, 0.0f));
}

/*------------------------------------------------------------------------------------------------------
| --- IntegrateVelocity: Updates linear and angular velocity using forces, damping, and speed caps --- |
------------------------------------------------------------------------------------------------------*/
void CaledonEngine::BodyIntegration2D::IntegrateVelocity(const BodyVelocityInput2D& input, float deltaTime, Vector2f& linearVelocity, float& angularVelocity)
{
	// Apply acceleration and accumulated forces/torques over the time step
	linearVelocity = input.linearVelocity + (input.acceleration + input.force * input.inverseMass) * deltaTime;
	angularVelocity = input.angularVelocity + input.torque * input.inverseInertia * Math::Rad2Deg * deltaTime;

	// Apply linear and angular damping decay
	linearVelocity = linearVelocity * (1.0f / (1.0f + deltaTime * std::fmax(input.linearDamping, 0.0f)));
	angularVelocity = ApplyDamping(angularVelocity, input.angularDamping, deltaTime);

	// Clamp linear velocity to the maximum translation speed limit if enabled
	if (input.maxTranslationSpeed > 0.0f)
	{
		const float speedSqr = linearVelocity.x * linearVelocity.x + linearVelocity.y * linearVelocity.y;
		if (speedSqr > input.maxTranslationSpeed * input.maxTranslationSpeed)
		{
			linearVelocity = linearVelocity * (input.maxTranslationSpeed / std::sqrt(speedSqr));
		}
	}

	// Clamp angular velocity to the maximum rotation speed limit if enabled
	if (input.maxRotationSpeed > 0.0f)
	{
		angularVelocity = std::fmax(-input.maxRotationSpeed, std::fmin(input.maxRotationSpeed, angularVelocity));
	}
}
