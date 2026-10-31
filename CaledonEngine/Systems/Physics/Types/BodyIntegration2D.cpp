/*------------------------------
| File: BodyIntegration2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "BodyIntegration2D.h"

#include "Utilities/Math/Constants.h"

#include <cmath>

/*--------------------------------------------------------------------------------------------------
| --- ApplyDamping: 
--------------------------------------------------------------------------------------------------*/
float CaledonEngine::BodyIntegration2D::ApplyDamping(float velocity, float damping, float deltaTime) noexcept
{
	return velocity / (1.0f + deltaTime * std::fmax(damping, 0.0f));
}

/*--------------------------------------------------------------------------------------------------
| --- IntegrateVelocity: 
--------------------------------------------------------------------------------------------------*/
void CaledonEngine::BodyIntegration2D::IntegrateVelocity(const BodyVelocityInput2D& input, float deltaTime, Vector2f& linearVelocity, float& angularVelocity)
{
	linearVelocity = input.linearVelocity + (input.acceleration + input.force * input.inverseMass) * deltaTime;
	angularVelocity = input.angularVelocity + input.torque * input.inverseInertia * Math::Rad2Deg * deltaTime;

	linearVelocity = linearVelocity * (1.0f / (1.0f + deltaTime * std::fmax(input.linearDamping, 0.0f)));
	angularVelocity = ApplyDamping(angularVelocity, input.angularDamping, deltaTime);

	if (input.maxTranslationSpeed > 0.0f)
	{
		const float speedSqr = linearVelocity.x * linearVelocity.x + linearVelocity.y * linearVelocity.y;
		if (speedSqr > input.maxTranslationSpeed * input.maxTranslationSpeed)
		{
			linearVelocity = linearVelocity * (input.maxTranslationSpeed / std::sqrt(speedSqr));
		}
	}

	if (input.maxRotationSpeed > 0.0f)
	{
		angularVelocity = std::fmax(-input.maxRotationSpeed, std::fmin(input.maxRotationSpeed, angularVelocity));
	}
}
