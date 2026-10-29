/*------------------------------
| File: Physics2DSubsystem.cpp
| Author: Chandler Mays
------------------------------*/
#include "Physics2DSubsystem.h"

namespace
{
	/*----------------------------------------------------------------------------
	| --- IsValidLayer: 
	----------------------------------------------------------------------------*/
	constexpr bool IsValidLayer(int layer) noexcept
	{
		return layer >= 0 && layer < CaledonEngine::Physics2DConstants::kLayerCount;
	}
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*----------------------------------------------------------------------------
| --- Constructor: Constructs the Physics2DSubsystem with default values --- |
----------------------------------------------------------------------------*/
CaledonEngine::Physics2DSubsystem::Physics2DSubsystem()
	: m_settings{ }
	, m_defaultScene{ m_settings }
{ }

/*----------------------------------------------------------------------------
| --- Name: 
----------------------------------------------------------------------------*/
std::string_view CaledonEngine::Physics2DSubsystem::Name() const noexcept
{
	return kName;
}

/*----------------------------------------------------------------------------
| --- Initialize:
----------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::Initialize(EngineContext& context)
{
	//...
	return true;
}

/*----------------------------------------------------------------------------
| --- Shutdown:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::Shutdown() noexcept
{
	m_defaultScene.Invalidate();
}

/*----------------------------------------------------------------------------
| --- Update:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::Update(float deltaTime)
{
	if (m_settings.simulationMode == SimulationMode2D::Update)
	{
		// Step the simulation once per frame when the simulation mode is == Update
		// StepScene(m_defaultScene, deltaTime, m_settings.simulationLayers) or similar?
	}
}

/*----------------------------------------------------------------------------
| --- FixedUpdate:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::FixedUpdate(float fixedDeltaTime)
{
	if (m_settings.simulationMode == SimulationMode2D::FixedUpdate)
	{
		// Step the simulation once per frame when the simulation mode is == Update
		// StepScene(m_defaultScene, fixedDeltaTime, m_settings.simulationLayers) or similar?
	}
}

/*----------------------------------------------------------------------------
| --- SetSettings:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetSettings(const Physics2DSettings& settings)
{
	m_settings = settings;
	m_defaultScene.WakeAllBodies();		// Update the bodies of the new settings configuration
}

/*----------------------------------------------------------------------------
| --- SetGravity:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetGravity(const Vector2f& gravity)
{
	m_settings.gravity = gravity;
	m_defaultScene.WakeAllBodies();		// A sleeping body would otherwise ignore the updated gravity
}

/*----------------------------------------------------------------------------
| --- SetPositionIterations:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetPositionIterations(int iterations)
{
	m_settings.positionIterations = std::max(1, iterations);
}

/*----------------------------------------------------------------------------
| --- SetVelocityIterations:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetVelocityIterations(int iterations)
{
	m_settings.velocityIterations = std::max(1, iterations);
}

/*----------------------------------------------------------------------------
| --- SetMaxSubStepCount:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetMaxSubStepCount(int count)
{
	m_settings.maxSubStepCount = std::max(1, count);
}

/*----------------------------------------------------------------------------
| --- SetMinSubStepFPS:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetMinSubStepFPS(float fps)
{
	if (fps > 0.0f)
	{
		m_settings.minSubStepFPS = fps;
	}
}

/*----------------------------------------------------------------------------
| --- BoxCast:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return m_defaultScene.BoxCast(origin, size, angle, direction, contactFilter, distance);
}

/*----------------------------------------------------------------------------
| --- BoxCast:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return m_defaultScene.BoxCast(origin, size, angle, direction, contactFilter, results, distance);
}

/*----------------------------------------------------------------------------
| --- BoxCastAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::BoxCastAll(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.BoxCastAll(origin, size, angle, direction, distance, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- CapsuleCast:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return m_defaultScene.CapsuleCast(origin, size, capsuleDirection, angle, direction, contactFilter, distance);
}

/*----------------------------------------------------------------------------
| --- CapsuleCast:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return m_defaultScene.CapsuleCast(origin, size, capsuleDirection, angle, direction, contactFilter, results, distance);
}

/*----------------------------------------------------------------------------
| --- CapsuleCastAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::CapsuleCastAll(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.CapsuleCastAll(origin, size, capsuleDirection, angle, direction, distance, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- CircleCast:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return m_defaultScene.CircleCast(origin, radius, direction, contactFilter, distance);
}

/*----------------------------------------------------------------------------
| --- CircleCast:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return m_defaultScene.CircleCast(origin, radius, direction, contactFilter, results, distance);
}

/*----------------------------------------------------------------------------
| --- CircleCastAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::CircleCastAll(const Vector2f& origin, float radius, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.CircleCastAll(origin, radius, direction, distance, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- Raycast:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return m_defaultScene.Raycast(origin, direction, contactFilter, distance);
}

/*----------------------------------------------------------------------------
| --- Raycast:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return m_defaultScene.Raycast(origin, direction, contactFilter, results, distance);
}

/*----------------------------------------------------------------------------
| --- RaycastAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::RaycastAll(const Vector2f& origin, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.RaycastAll(origin, direction, distance, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- Linecast:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.Linecast(start, end, contactFilter);
}

/*----------------------------------------------------------------------------
| --- Linecast:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return m_defaultScene.Linecast(start, end, contactFilter, results);
}

/*----------------------------------------------------------------------------
| --- LinecastAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::LinecastAll(const Vector2f& start, const Vector2f& end, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.LinecastAll(start, end, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- OverlapArea:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapArea(pointA, pointB, contactFilter);
}

/*----------------------------------------------------------------------------
| --- OverlapArea:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapArea(pointA, pointB, contactFilter, results);
}

/*----------------------------------------------------------------------------
| --- OverlapAreaAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapAreaAll(const Vector2f& pointA, const Vector2f& pointB, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapAreaAll(pointA, pointB, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- OverlapBox:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapBox(point, size, angle, contactFilter);
}

/*----------------------------------------------------------------------------
| --- OverlapBox:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapBox(point, size, angle, contactFilter, results);
}

/*----------------------------------------------------------------------------
| --- OverlapBoxAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapBoxAll(const Vector2f& point, const Vector2f& size, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapBoxAll(point, size, angle, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- OverlapCapsule:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapCapsule(point, size, capsuleDirection, angle, contactFilter);
}

/*----------------------------------------------------------------------------
| --- OverlapCapsule:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapCapsule(point, size, capsuleDirection, angle, contactFilter, results);
}

/*----------------------------------------------------------------------------
| --- OverlapCapsuleAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCapsuleAll(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapCapsuleAll(point, size, capsuleDirection, angle, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- OverlapCircle:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapCircle(point, radius, contactFilter);
}

/*----------------------------------------------------------------------------
| --- OverlapCircle:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapCircle(point, radius, contactFilter, results);
}

/*----------------------------------------------------------------------------
| --- OverlapCircleAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCircleAll(const Vector2f& point, float radius, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapCircleAll(point, radius, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- OverlapCollider:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapCollider(pCollider, contactFilter);
}

/*----------------------------------------------------------------------------
| --- OverlapCollider:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapCollider(pCollider, contactFilter, results);
}

/*----------------------------------------------------------------------------
| --- OverlapPoint:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapPoint(point, contactFilter);
}

/*----------------------------------------------------------------------------
| --- OverlapPoint:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapPoint(point, contactFilter, results);
}

/*----------------------------------------------------------------------------
| --- OverlapPointAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapPointAll(const Vector2f& point, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapPointAll(point, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------
| --- GetRayIntersection:
----------------------------------------------------------------------------*/
CaledonEngine::ColliderHit2D CaledonEngine::Physics2DSubsystem::GetRayIntersection(const Ray2D& ray, float distance, int layerMask) const
{
	auto hits = m_defaultScene.GetRayIntersection(ray, distance, layerMask);
	return hits.empty() ? ColliderHit2D{} : hits.front();
}

/*----------------------------------------------------------------------------
| --- GetRayIntersectionAll:
----------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::GetRayIntersectionAll(const Ray2D& ray, float distance, int layerMask) const
{
	return m_defaultScene.GetRayIntersectionAll(ray, distance, layerMask);
}

/*----------------------------------------------------------------------------
| --- ClosestPoint:
----------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Physics2DSubsystem::ClosestPoint(const Vector2f& position, const Collider2D* pCollider) const
{
	return m_defaultScene.ClosestPoint(position, pCollider);
}

/*----------------------------------------------------------------------------
| --- Distance:
----------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::Physics2DSubsystem::Distance(const Collider2D* pColliderA, const Collider2D* pColliderB) const
{
	return m_defaultScene.Distance(pColliderA, pColliderB);
}

/*----------------------------------------------------------------------------
| --- GetContacts:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::GetContacts(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
	// TODO: Implement -> If tracking contacts via the scene, forward here. Otherwise, remove the method.
}

/*----------------------------------------------------------------------------
| --- GetContactColliders:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::GetContactColliders(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapCollider(pCollider, contactFilter, results);
}

/*----------------------------------------------------------------------------
| --- GetIgnoreCollision:
----------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::GetIgnoreCollision(const Collider2D* pCollider1, const Collider2D* pCollider2) const
{
	return m_defaultScene.GetIgnoreCollision(pCollider1, pCollider2);
}

/*----------------------------------------------------------------------------
| --- GetIgnoreLayerCollision:
----------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::GetIgnoreLayerCollision(int layer1, int layer2) const
{
	if (!IsValidLayer(layer1) || !IsValidLayer(layer2))
		return false;

	return (static_cast<uint32_t>(m_settings.layerCollisionMatrix[layer1]) & (1u << layer2)) == 0;
}

/*----------------------------------------------------------------------------
| --- GetLayerCollisionMask:
----------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::GetLayerCollisionMask(int layer) const
{
	return IsValidLayer(layer) ? m_settings.layerCollisionMatrix[layer] : 0;
}

/*----------------------------------------------------------------------------
| --- IgnoreCollision:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::IgnoreCollision(Collider2D* pCollider1, Collider2D* pCollider2, bool ignore)
{
	m_defaultScene.SetIgnoreCollision(pCollider1, pCollider2, ignore);
}

/*----------------------------------------------------------------------------
| --- IgnoreLayerCollision:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::IgnoreLayerCollision(int layer1, int layer2, bool ignore)
{
	if (!IsValidLayer(layer1) || !IsValidLayer(layer2))
		return;

	auto& matrix = m_settings.layerCollisionMatrix;
	const uint32_t bit1 = 1u << layer1;
	const uint32_t bit2 = 1u << layer2;

	// Keep the matrix symmetric: update both directions
	if (ignore)
	{
		matrix[layer1] = static_cast<int>(static_cast<uint32_t>(matrix[layer1]) & ~bit2);
		matrix[layer2] = static_cast<int>(static_cast<uint32_t>(matrix[layer2]) & ~bit1);
	}
	else
	{
		matrix[layer1] = static_cast<int>(static_cast<uint32_t>(matrix[layer1]) | bit2);
		matrix[layer2] = static_cast<int>(static_cast<uint32_t>(matrix[layer2]) | bit1);
	}
}

/*----------------------------------------------------------------------------
| --- IsTouching:
----------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::IsTouching(const Collider2D* pCollider1, const Collider2D* pCollider2) const
{
	return m_defaultScene.IsTouching(pCollider1, pCollider2);
}

/*----------------------------------------------------------------------------
| --- IsTouching:
----------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.IsTouching(pCollider, contactFilter);
}

/*----------------------------------------------------------------------------
| --- IsTouching:
----------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::IsTouching(const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.IsTouching(contactFilter);
}

/*----------------------------------------------------------------------------
| --- IsTouchingLayers:
----------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::IsTouchingLayers(const Collider2D* pCollider, LayerMask layerMask) const
{
	if (pCollider == nullptr)
		return false;

	ContactFilter2D filter = ContactFilter2D::NoFilter();
	filter.SetLayerMask(layerMask);
	return m_defaultScene.IsTouching(pCollider, filter);
}

/*----------------------------------------------------------------------------
| --- SetLayerCollisionMask:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetLayerCollisionMask(int layer, int layerMask)
{
	if (!IsValidLayer(layer))
		return;

	auto& matrix = m_settings.layerCollisionMatrix;
	matrix[layer] = layerMask;

	// Keep the matrix symmetric: every other layer's bit for this layer must match the new mask
	const uint32_t layerBit = 1u << layer;
	for (int other = 0; other < Physics2DConstants::kLayerCount; ++other)
	{
		const bool collides = (static_cast<uint32_t>(layerMask) & (1u << other)) != 0;
		const uint32_t otherMask = static_cast<uint32_t>(matrix[other]);
		matrix[other] = static_cast<int>(collides ? (otherMask | layerBit) : (otherMask & ~layerBit));
	}
}

/*----------------------------------------------------------------------------
| --- Simulate:
----------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::Simulate(float deltaTime, int simulatedLayers)
{
	return m_defaultScene.Simulate(deltaTime, simulatedLayers);
}

/*----------------------------------------------------------------------------
| --- SyncTransforms:
----------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SyncTransforms()
{
	m_defaultScene.SyncTransforms();
}