/*------------------------------
| File: ContactSolver2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "ContactSolver2D.h"

#include <algorithm>
#include <cmath>

namespace
{
	using CaledonEngine::SolverBody2D;
	using CaledonEngine::Vector2f;

	float Cross(const Vector2f& a, const Vector2f& b) noexcept { return a.x * b.y - a.y * b.x; }
	float Dot(const Vector2f& a, const Vector2f& b) noexcept { return a.x * b.x + a.y * b.y; }
	Vector2f Cross(float s, const Vector2f& v) noexcept { return Vector2f{ -s * v.y, s * v.x }; }
	Vector2f Rotate(const Vector2f& v, float angle) noexcept
	{
		const float c = std::cos(angle);
		const float s = std::sin(angle);
		return Vector2f{ c * v.x - s * v.y, s * v.x + c * v.y };
	}

	/*---------------------------------------------------------------------------------------------
	| --- InverseMassAlong: Calculates the inverse mass of a body along a specified direction --- |
	---------------------------------------------------------------------------------------------*/
	float InverseMassAlong(const SolverBody2D& body, const Vector2f& direction) noexcept
	{
		return body.inverseMass * (direction.x * direction.x * body.linearFactor.x + direction.y * direction.y * body.linearFactor.y);
	}

	/*-------------------------------------------------------------------------------------------------------
	| --- ApplyImpulse: Applies an impulse to two bodies, modifying their linear and angular velocities --- |
	-------------------------------------------------------------------------------------------------------*/
	void ApplyImpulse(SolverBody2D& a, SolverBody2D& b, const Vector2f& rA, const Vector2f& rB, const Vector2f& impulse) noexcept
	{
		a.linearVelocity -= Vector2f{ a.linearFactor.x * impulse.x, a.linearFactor.y * impulse.y } * a.inverseMass;
		a.angularVelocity -= a.inverseInertia * Cross(rA, impulse);
		b.linearVelocity += Vector2f{ b.linearFactor.x * impulse.x, b.linearFactor.y * impulse.y } * b.inverseMass;
		b.angularVelocity += b.inverseInertia * Cross(rB, impulse);
	}

	Vector2f RelativeVelocity(const SolverBody2D& a, const SolverBody2D& b, const Vector2f& rA, const Vector2f& rB) noexcept
	{
		return (b.linearVelocity + Cross(b.angularVelocity, rB)) - (a.linearVelocity + Cross(a.angularVelocity, rA));
	}

	float Reciprocal(float value) noexcept { return value > 0.0f ? 1.0f / value : 0.0f; }
}

/*-------------------------------------------------------------------------------------------------------------------------------------------
| --- Build: Builds a contact constraint from a manifold, including effective masses, restitution bias and opitonal warm-start impulses --- |
-------------------------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::ContactConstraint2D CaledonEngine::ContactSolver2D::Build(const ShapeManifold2D& manifold, float friction, float restitution, int bodyA, int bodyB,
	std::span<const SolverBody2D> bodies, const ContactSolverSettings2D& settings, const ContactImpulses2D* warm)
{
	ContactConstraint2D constraint;
	constraint.bodyA = bodyA;
	constraint.bodyB = bodyB;
	constraint.normal = manifold.normal;
	constraint.friction = friction;
	constraint.restitution = restitution;
	constraint.pointCount = std::clamp(manifold.pointCount, 0, 2);

	const SolverBody2D& a = bodies[static_cast<std::size_t>(bodyA)];
	const SolverBody2D& b = bodies[static_cast<std::size_t>(bodyB)];
	constraint.angleA = a.angle;
	constraint.angleB = b.angle;

	const Vector2f normal = manifold.normal;
	const Vector2f tangent{ normal.y, -normal.x };

	for (int i = 0; i < constraint.pointCount; ++i)
	{
		ContactPointConstraint2D& point = constraint.points[i];
		point.anchorA = manifold.points[i].point - a.position;
		point.anchorB = manifold.points[i].point - b.position;
		point.separation = manifold.points[i].separation;

		const float rnA = Cross(point.anchorA, normal);
		const float rnB = Cross(point.anchorB, normal);
		point.normalMass = Reciprocal(InverseMassAlong(a, normal) + InverseMassAlong(b, normal) + a.inverseInertia * rnA * rnA + b.inverseInertia * rnB * rnB);

		const float rtA = Cross(point.anchorA, tangent);
		const float rtB = Cross(point.anchorB, tangent);
		point.tangentMass = Reciprocal(InverseMassAlong(a, tangent) + InverseMassAlong(b, tangent) + a.inverseInertia * rtA * rtA + b.inverseInertia * rtB * rtB);

		const float closing = Dot(normal, RelativeVelocity(a, b, point.anchorA, point.anchorB));
		point.velocityBias = (closing < -settings.bounceThreshold) ? -restitution * closing : 0.0f;

		if (warm != nullptr)
		{
			point.normalImpulse = std::max(warm->normal[i], 0.0f);
			point.tangentImpulse = warm->tangent[i];
		}
	}

	return constraint;
}

/*----------------------------------------------------------------------------------
| --- WarmStart: Applies previously accumulated contact impulses to the bodies --- |
----------------------------------------------------------------------------------*/
void CaledonEngine::ContactSolver2D::WarmStart(std::span<const ContactConstraint2D> constraints, std::span<SolverBody2D> bodies)
{
	for (const ContactConstraint2D& constraint : constraints)
	{
		SolverBody2D& a = bodies[static_cast<std::size_t>(constraint.bodyA)];
		SolverBody2D& b = bodies[static_cast<std::size_t>(constraint.bodyB)];
		const Vector2f tangent{ constraint.normal.y, -constraint.normal.x };

		for (int i = 0; i < constraint.pointCount; ++i)
		{
			const ContactPointConstraint2D& point = constraint.points[i];
			ApplyImpulse(a, b, point.anchorA, point.anchorB, constraint.normal * point.normalImpulse + tangent * point.tangentImpulse);
		}
	}
}

/*----------------------------------------------------------------------------------------------------
| --- SolveVelocity: Iteratively resolves relative velocities along contact normals and tangents --- |
----------------------------------------------------------------------------------------------------*/
void CaledonEngine::ContactSolver2D::SolveVelocity(std::span<ContactConstraint2D> constraints, std::span<SolverBody2D> bodies)
{
	for (ContactConstraint2D& constraint : constraints)
	{
		SolverBody2D& a = bodies[static_cast<std::size_t>(constraint.bodyA)];
		SolverBody2D& b = bodies[static_cast<std::size_t>(constraint.bodyB)];
		const Vector2f normal = constraint.normal;
		const Vector2f tangent{ normal.y, -normal.x };

		// Friction: remove the sliding speed, but never push harder than friction * (the normal impulse so far)
		for (int i = 0; i < constraint.pointCount; ++i)
		{
			ContactPointConstraint2D& point = constraint.points[i];
			const float slide = Dot(tangent, RelativeVelocity(a, b, point.anchorA, point.anchorB));
			const float maxFriction = constraint.friction * point.normalImpulse;
			const float newImpulse = std::clamp(point.tangentImpulse - point.tangentMass * slide, -maxFriction, maxFriction);
			ApplyImpulse(a, b, point.anchorA, point.anchorB, tangent * (newImpulse - point.tangentImpulse));
			point.tangentImpulse = newImpulse;
		}

		// Normal: remove the closing speed (aiming for the bounce speed), but a contact can only push, never pull
		for (int i = 0; i < constraint.pointCount; ++i)
		{
			ContactPointConstraint2D& point = constraint.points[i];
			const float closing = Dot(normal, RelativeVelocity(a, b, point.anchorA, point.anchorB));
			const float newImpulse = std::max(point.normalImpulse - point.normalMass * (closing - point.velocityBias), 0.0f);
			ApplyImpulse(a, b, point.anchorA, point.anchorB, normal * (newImpulse - point.normalImpulse));
			point.normalImpulse = newImpulse;
		}
	}
}

/*--------------------------------------------------------------------------------------------------------------------
| --- SolvePosition: Corrects positional overlap and returns whether the constraints have sufficiently converged --- |
--------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::ContactSolver2D::SolvePosition(std::span<const ContactConstraint2D> constraints, std::span<SolverBody2D> bodies, const ContactSolverSettings2D& settings)
{
	float minSeparation = 0.0f;

	for (const ContactConstraint2D& constraint : constraints)
	{
		SolverBody2D& a = bodies[static_cast<std::size_t>(constraint.bodyA)];
		SolverBody2D& b = bodies[static_cast<std::size_t>(constraint.bodyB)];
		const Vector2f normal = constraint.normal;

		for (int i = 0; i < constraint.pointCount; ++i)
		{
			const ContactPointConstraint2D& point = constraint.points[i];
			const Vector2f rA = Rotate(point.anchorA, a.angle - constraint.angleA);
			const Vector2f rB = Rotate(point.anchorB, b.angle - constraint.angleB);
			const float separation = point.separation + Dot((b.position + rB) - (a.position + rA), normal);
			minSeparation = std::min(minSeparation, separation);

			const float correction = std::clamp(settings.baumgarte * (separation + settings.linearSlop), -settings.maxLinearCorrection, 0.0f);
			const float rnA = Cross(rA, normal);
			const float rnB = Cross(rB, normal);
			const float effectiveMass = Reciprocal(InverseMassAlong(a, normal) + InverseMassAlong(b, normal) + a.inverseInertia * rnA * rnA + b.inverseInertia * rnB * rnB);
			const Vector2f impulse = normal * (-correction * effectiveMass);

			a.position -= Vector2f{ a.linearFactor.x * impulse.x, a.linearFactor.y * impulse.y } * a.inverseMass;
			a.angle -= a.inverseInertia * Cross(rA, impulse);
			b.position += Vector2f{ b.linearFactor.x * impulse.x, b.linearFactor.y * impulse.y } * b.inverseMass;
			b.angle += b.inverseInertia * Cross(rB, impulse);
		}
	}

	return minSeparation >= -3.0f * settings.linearSlop;
}

/*------------------------------------------------------------------------------------------------------
| --- GetImpulses: Retrieves the accumulated normal and frition impulses from a contact constraint --- |
------------------------------------------------------------------------------------------------------*/
CaledonEngine::ContactImpulses2D CaledonEngine::ContactSolver2D::GetImpulses(const ContactConstraint2D& constraint) noexcept
{
	ContactImpulses2D impulses;
	for (int i = 0; i < constraint.pointCount; ++i)
	{
		impulses.normal[i] = constraint.points[i].normalImpulse;
		impulses.tangent[i] = constraint.points[i].tangentImpulse;
	}
	return impulses;
}
