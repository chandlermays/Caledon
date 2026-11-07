/*------------------------------
| File: BodySlide2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "BodySlide2D.h"

#include "Utilities/Math/Constants.h"

#include <algorithm>
#include <cmath>

namespace
{
	using namespace CaledonEngine;

	constexpr float kAngleTolerance = 0.001f;
	constexpr float kMinApproach = 0.1f;

	/*------------------------------------------------------------------------------------
	| --- AngleToUp: Returns the angle between a surface normal and the up direction --- |
	------------------------------------------------------------------------------------*/
	float AngleToUp(const Vector2f& normal, const Vector2f& up) noexcept
	{
		return std::acos(std::clamp(Vector2f::Dot(normal, up), -1.0f, 1.0f)) * Math::Rad2Deg;
	}

	/*------------------------------------------------------------------------------------------------
	| --- TravelToHit: Returns the distance to travel along a direction before hitting a surface --- |
	------------------------------------------------------------------------------------------------*/
	float TravelToHit(const ColliderHit2D& hit, const Vector2f& direction, float skin) noexcept
	{
		const float approach = std::max(-Vector2f::Dot(direction, hit.normal), kMinApproach);
		return std::clamp(hit.distance - skin / approach, 0.0f, hit.distance);
	}
}

/*------------------------------------------------------------------------------------------------------------
| --- Compute: Computes the final position and remaining velocity of a body after sliding along surfaces --- |
------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::BodySlide2D::Compute(const SlideInput2D& input, const SlideCastFunction& cast, SlideResults2D& results)
{
	results = SlideResults2D{ };
	results.position = input.position;
	results.remainingVelocity = input.velocity;

	if (!(input.deltaTime > 0.0f) || input.maxIterations <= 0 || !cast)
		return;

	Vector2f up = input.surfaceUp;
	if (!(up.SqrMagnitude() > 0.0f))
	{
		up = Vector2f{ 0.0f, 1.0f };
	}
	up = up.Normalized();

	Vector2f position = input.position;
	Vector2f remaining = input.velocity * input.deltaTime;
	const bool movementRequested = remaining.Magnitude() > Math::LooseTolerance;
	int iterations = 0;
	Vector2f unused{ };

	for (int i = 0; i < input.maxIterations; ++i)
	{
		const float distance = remaining.Magnitude();
		if (distance <= Math::TightTolerance)
			break;

		const Vector2f direction = remaining * (1.0f / distance);
		++iterations;

		ColliderHit2D hit;
		if (!cast(position, direction, distance, hit))
		{
			position = position + remaining;
			remaining = Vector2f{ 0.0f, 0.0f };
			break;
		}

		const float travelled = TravelToHit(hit, direction, input.skin);
		position = position + direction * travelled;
		const Vector2f rest = remaining - direction * travelled;

		if (AngleToUp(hit.normal, up) < input.surfaceSlideAngle - kAngleTolerance)
		{
			results.slideHit = hit;
			remaining = rest - hit.normal * Vector2f::Dot(rest, hit.normal);
		}
		else
		{
			unused = rest;
			remaining = Vector2f{ 0.0f, 0.0f };
			break;
		}
	}
	unused = unused + remaining;

	Vector2f gravityMove = input.gravity * input.deltaTime;
	const int gravityBudget = std::max(input.maxIterations - iterations, 1);
	for (int i = 0; i < gravityBudget; ++i)
	{
		const float distance = gravityMove.Magnitude();
		if (distance <= Math::TightTolerance)
			break;

		const Vector2f direction = gravityMove * (1.0f / distance);
		++iterations;

		ColliderHit2D hit;
		if (!cast(position, direction, distance, hit))
		{
			position = position + gravityMove;
			break;
		}

		const float travelled = TravelToHit(hit, direction, input.skin);
		position = position + direction * travelled;
		const Vector2f rest = gravityMove - direction * travelled;
		results.surfaceHit = hit;

		if (!movementRequested && AngleToUp(hit.normal, up) > input.gravitySlipAngle + kAngleTolerance)
		{
			gravityMove = rest - hit.normal * Vector2f::Dot(rest, hit.normal);
			continue;
		}
		break;
	}

	const float anchorDistance = input.surfaceAnchor.Magnitude();
	if (!results.surfaceHit.IsValid() && anchorDistance > Math::TightTolerance)
	{
		const Vector2f direction = input.surfaceAnchor * (1.0f / anchorDistance);
		ColliderHit2D hit;
		if (cast(position, direction, anchorDistance, hit))
		{
			position = position + direction * TravelToHit(hit, direction, input.skin);
			results.surfaceHit = hit;
		}
	}

	results.position = position;
	results.remainingVelocity = unused * (1.0f / input.deltaTime);
	results.iterationsUsed = iterations;
}
