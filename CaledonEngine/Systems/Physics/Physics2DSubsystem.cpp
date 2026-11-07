/*------------------------------
| File: Physics2DSubsystem.cpp
| Author: Chandler Mays
------------------------------*/
#include "Physics2DSubsystem.h"

namespace
{
	/*-----------------------------------------------------------------------------
	| --- IsValidLayer: Returns true if the given layer index is valid (0-31) --- |
	-----------------------------------------------------------------------------*/
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

/*-------------------------------------------------
| --- Name: Returns the name of the subsystem --- |
-------------------------------------------------*/
std::string_view CaledonEngine::Physics2DSubsystem::Name() const noexcept
{
	return kName;
}

/*-------------------------------------------------------------
| --- Initialize: Prepares the Physics2DSubsystem for use --- |
-------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::Initialize(EngineContext& context)
{
	//...
	return true;
}

/*-------------------------------------------------------------------------------
| --- Shutdown: Cleans up the Physics2DSubsystem and releases any resources --- |
-------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::Shutdown() noexcept
{
	m_defaultScene.Invalidate();
}

/*-----------------------------------------------------------------------------------------------------------------------
| --- Update: Updates the Physics2DSubsystem and its default PhysicsScene2D if the simulation mode is set to Update --- |
-----------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::Update(float deltaTime)
{
	if (m_settings.simulationMode == SimulationMode2D::Update)
	{
		// Step the simulation once per frame when the simulation mode is == Update
		m_defaultScene.Simulate(deltaTime, m_settings.simulationLayers);
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------
| --- FixedUpdate: Updates the Physics2DSubsystem and its default PhysicsScene2D if the simulation mode is set to FixedUpdate --- |
---------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::FixedUpdate(float fixedDeltaTime)
{
	if (m_settings.simulationMode == SimulationMode2D::FixedUpdate)
	{
		// Step the simulation once per fixed update when the simulation mode is == FixedUpdate
		m_defaultScene.Simulate(fixedDeltaTime, m_settings.simulationLayers);
	}
}

/*-------------------------------------------------------------------------------------------------------------------------
| --- SetSettings: Sets the physics settings for the Physics2DSubsystem and its default PhysicsScene2D (write access) --- |
-------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetSettings(const Physics2DSettings& settings)
{
	m_settings = settings;
	m_defaultScene.WakeAllBodies();		// Update the bodies of the new settings configuration
}

/*----------------------------------------------------------------------------------
| --- SetGravity: Sets the global gravity vector for the 2D physics simulation --- |
----------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetGravity(const Vector2f& gravity)
{
	m_settings.gravity = gravity;
	m_defaultScene.WakeAllBodies();		// A sleeping body would otherwise ignore the updated gravity
}

/*------------------------------------------------------------------------------------------------------------------------
| --- SetPositionIterations: Sets the number of iterations of the physics solver when considering objects' positions --- |
------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetPositionIterations(int iterations)
{
	m_settings.positionIterations = std::max(1, iterations);
}

/*-------------------------------------------------------------------------------------------------------------------------
| --- SetVelocityIterations: Sets the number of iterations of the physics solver when considering objects' velocities --- |
-------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetVelocityIterations(int iterations)
{
	m_settings.velocityIterations = std::max(1, iterations);
}

/*---------------------------------------------------------------------------------------------------------------------------------------
| --- SetMaxSubStepCount: Sets the maximum number of simulation sub-steps allowed per-frame when simulation sub-stepping is enabled --- |
---------------------------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetMaxSubStepCount(int count)
{
	m_settings.maxSubStepCount = std::max(1, count);
}

/*---------------------------------------------------------------------------------------------------------------
| --- SetMinSubStepFPS: Sets the minimum FPS allowed for a simulation step before sub-stepping will be used --- |
---------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetMinSubStepFPS(float fps)
{
	if (fps > 0.0f)
	{
		m_settings.minSubStepFPS = fps;
	}
}

/*-----------------------------------------------------------------------------------------------
| --- GetIgnoreCollision: Checks whether collision between two Collider2D are to be ignored --- |
-----------------------------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::GetIgnoreCollision(const Collider2D* pCollider1, const Collider2D* pCollider2) const
{
	return m_defaultScene.GetIgnoreCollision(pCollider1, pCollider2);
}

/*--------------------------------------------------------------------------------------------
| --- SetIgnoreCollision: Sets a collision between two Collider2D to be ignored (or not) --- |
--------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetIgnoreCollision(Collider2D* pCollider1, Collider2D* pCollider2, bool ignore)
{
	m_defaultScene.SetIgnoreCollision(pCollider1, pCollider2, ignore);
}

/*------------------------------------------------------------------------------------------------------------
| --- GetIgnoreLayerCollision: Checks whether collisions between the specified layers are ignored or not --- |
------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::GetIgnoreLayerCollision(int layer1, int layer2) const
{
	if (!IsValidLayer(layer1) || !IsValidLayer(layer2))
		return false;

	return (static_cast<uint32_t>(m_settings.layerCollisionMatrix[layer1]) & (1u << layer2)) == 0;
}

/*--------------------------------------------------------------------------------------------------------------------
| --- SetIgnoreLayerCollision: Chooses whether to detect or ignore collisions between a specified pair of layers --- |
--------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SetIgnoreLayerCollision(int layer1, int layer2, bool ignore)
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

/*---------------------------------------------------------------------------------------------------------------------------------
| --- GetLayerCollisionMask: Gets the collision layer mask that indicates which layer(s) the specified layer can collide with --- |
---------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::GetLayerCollisionMask(int layer) const
{
	return IsValidLayer(layer) ? m_settings.layerCollisionMatrix[layer] : 0;
}

/*---------------------------------------------------------------------------------------------------------------------------------
| --- SetLayerCollisionMask: Sets the collision layer mask that indicates which layer(s) the specified layer can collide with --- |
---------------------------------------------------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------------------------------------------------------
| --- GetContacts: Retrieves all of the ContactPoint2D with the given Collider2D, with the results filtered by the ContactFilter2D --- |
--------------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::GetContacts(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
	return m_defaultScene.GetContacts(pCollider, contactFilter, contacts);
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------
| --- GetContactColliders: Retrieves all of the unique Collider2D in contact with the given Collider2D, with the results filtered by the ContactFilter2D --- |
------------------------------------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::GetContactColliders(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.GetContactColliders(pCollider, contactFilter, results);
}

/*-----------------------------------------------------------------------------------------------
| --- IsTouching: Returns whether the two Collider2D were in contact during the latest step --- |
-----------------------------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::IsTouching(const Collider2D* pCollider1, const Collider2D* pCollider2) const
{
	return m_defaultScene.IsTouching(pCollider1, pCollider2);
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------
| --- IsTouching: Returns whether the given Collider2D was touching any other Collider2D that passes the ContactFilter2D during the latest step --- |
---------------------------------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.IsTouching(pCollider, contactFilter);
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: --- |
-------------------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::IsTouching(const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.IsTouching(contactFilter);
}

/*----------------------------------------------------------------------------------------------------------------------------
| --- IsTouchingLayers: Returns whether the given Collider2D is touching any other Collider2D on the specified LayerMask --- |
----------------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::IsTouchingLayers(const Collider2D* pCollider, LayerMask layerMask) const
{
	if (pCollider == nullptr)
		return false;

	ContactFilter2D filter = ContactFilter2D::NoFilter();
	filter.SetLayerMask(layerMask);
	return m_defaultScene.IsTouching(pCollider, filter);
}

/*--------------------------------------------------------------------------------------------------------------
| --- BoxCast: Returns all intersections of a casted Box against the Collider2D in this Physics2DSubsystem --- |
--------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return m_defaultScene.BoxCast(origin, size, angle, direction, contactFilter, distance);
}

/*--------------------------------------------------------------------------------------------------------------
| --- BoxCast: Returns all intersections of a casted Box against the Collider2D in this Physics2DSubsystem --- |
--------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::BoxCast(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return m_defaultScene.BoxCast(origin, size, angle, direction, contactFilter, results, distance);
}

/*-------------------------------------------------------------------------------------------------------------------
| --- BoxCastAll: Returns all of the unique Collider2D intersected by the casted Box in this Physics2DSubsystem --- |
-------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::BoxCastAll(const Vector2f& origin, const Vector2f& size, float angle, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.BoxCastAll(origin, size, angle, direction, distance, layerMask, minDepth, maxDepth);
}

/*----------------------------------------------------------------------------------------------------------------------
| --- CapsuleCast: Returns all intersections of a casted Capsule against the Collider2D in this Physics2DSubsystem --- |
----------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return m_defaultScene.CapsuleCast(origin, size, capsuleDirection, angle, direction, contactFilter, distance);
}

/*----------------------------------------------------------------------------------------------------------------------
| --- CapsuleCast: Returns all intersections of a casted Capsule against the Collider2D in this Physics2DSubsystem --- |
----------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::CapsuleCast(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return m_defaultScene.CapsuleCast(origin, size, capsuleDirection, angle, direction, contactFilter, results, distance);
}

/*---------------------------------------------------------------------------------------------------------------------------
| --- CapsuleCastAll: Returns all of the unique Collider2D intersected by the casted Capsule in this Physics2DSubsystem --- |
---------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::CapsuleCastAll(const Vector2f& origin, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.CapsuleCastAll(origin, size, capsuleDirection, angle, direction, distance, layerMask, minDepth, maxDepth);
}

/*--------------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Collider2D in this Physics2DSubsystem --- |
--------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return m_defaultScene.CircleCast(origin, radius, direction, contactFilter, distance);
}

/*--------------------------------------------------------------------------------------------------------------------
| --- CircleCast: Returns all intersections of a casted Circle against the Collider2D in this Physics2DSubsystem --- |
--------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::CircleCast(const Vector2f& origin, float radius, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return m_defaultScene.CircleCast(origin, radius, direction, contactFilter, results, distance);
}

/*-------------------------------------------------------------------------------------------------------------------------
| --- CircleCastAll: Returns all of the unique Collider2D intersected by the casted Circle in this Physics2DSubsystem --- |
-------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::CircleCastAll(const Vector2f& origin, float radius, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.CircleCastAll(origin, radius, direction, distance, layerMask, minDepth, maxDepth);
}

/*--------------------------------------------------------------------------------------------------------------
| --- Raycast: Returns all intersections of a casted Ray against the Collider2D in this Physics2DSubsystem --- |
--------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, float distance) const
{
	return m_defaultScene.Raycast(origin, direction, contactFilter, distance);
}

/*--------------------------------------------------------------------------------------------------------------
| --- Raycast: Returns all intersections of a casted Ray against the Collider2D in this Physics2DSubsystem --- |
--------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::Raycast(const Vector2f& origin, const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance) const
{
	return m_defaultScene.Raycast(origin, direction, contactFilter, results, distance);
}

/*-------------------------------------------------------------------------------------------------------------------
| --- RaycastAll: Returns all of the unique Collider2D intersected by the casted Ray in this Physics2DSubsystem --- |
-------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::RaycastAll(const Vector2f& origin, const Vector2f& direction, float distance, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.RaycastAll(origin, direction, distance, layerMask, minDepth, maxDepth);
}

/*------------------------------------------------------------------------------------------------------------------------
| --- Linecast: Returns all intersections of a casted Line segment against the Collider2D in this Physics2DSubsystem --- |
------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.Linecast(start, end, contactFilter);
}

/*------------------------------------------------------------------------------------------------------------------------
| --- Linecast: Returns all intersections of a casted Line segment against the Collider2D in this Physics2DSubsystem --- |
------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::Linecast(const Vector2f& start, const Vector2f& end, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results) const
{
	return m_defaultScene.Linecast(start, end, contactFilter, results);
}

/*-----------------------------------------------------------------------------------------------------------------------------
| --- LinecastAll: Returns all of the unique Collider2D intersected by the casted Line segment in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::LinecastAll(const Vector2f& start, const Vector2f& end, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.LinecastAll(start, end, layerMask, minDepth, maxDepth);
}

/*-------------------------------------------------------------------------------------------------------------------------
| --- OverlapArea: Returns all intersections of an overlapping Area against the Collider2D in this Physics2DSubsystem --- |
-------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapArea(pointA, pointB, contactFilter);
}

/*-------------------------------------------------------------------------------------------------------------------------
| --- OverlapArea: Returns all intersections of an overlapping Area against the Collider2D in this Physics2DSubsystem --- |
-------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapArea(const Vector2f& pointA, const Vector2f& pointB, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapArea(pointA, pointB, contactFilter, results);
}

/*-----------------------------------------------------------------------------------------------------------------------------
| --- OverlapAreaAll: Returns all of the unique Collider2D intersected by the overlapping Area in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapAreaAll(const Vector2f& pointA, const Vector2f& pointB, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapAreaAll(pointA, pointB, layerMask, minDepth, maxDepth);
}

/*-----------------------------------------------------------------------------------------------------------------------
| --- OverlapBox: Returns all intersections of an overlapping Box against the Collider2D in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapBox(point, size, angle, contactFilter);
}

/*-----------------------------------------------------------------------------------------------------------------------
| --- OverlapBox: Returns all intersections of an overlapping Box against the Collider2D in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapBox(const Vector2f& point, const Vector2f& size, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapBox(point, size, angle, contactFilter, results);
}

/*---------------------------------------------------------------------------------------------------------------------------
| --- OverlapBoxAll: Returns all of the unique Collider2D intersected by the overlapping Box in this Physics2DSubsystem --- |
---------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapBoxAll(const Vector2f& point, const Vector2f& size, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapBoxAll(point, size, angle, layerMask, minDepth, maxDepth);
}

/*-------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Collider2D in this Physics2DSubsystem --- |
-------------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapCapsule(point, size, capsuleDirection, angle, contactFilter);
}

/*-------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsule: Returns all intersections of an overlapping Capsule against the Collider2D in this Physics2DSubsystem --- |
-------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapCapsule(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapCapsule(point, size, capsuleDirection, angle, contactFilter, results);
}

/*-----------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCapsuleAll: Returns all of the unique Collider2D intersected by the overlapping Capsule in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCapsuleAll(const Vector2f& point, const Vector2f& size, CapsuleDirection2D capsuleDirection, float angle, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapCapsuleAll(point, size, capsuleDirection, angle, layerMask, minDepth, maxDepth);
}

/*-----------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Collider2D in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapCircle(point, radius, contactFilter);
}

/*-----------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircle: Returns all intersections of an overlapping Circle against the Collider2D in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapCircle(const Vector2f& point, float radius, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapCircle(point, radius, contactFilter, results);
}

/*---------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCircleAll: Returns all of the unique Collider2D intersected by the overlapping Circle in this Physics2DSubsystem --- |
---------------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCircleAll(const Vector2f& point, float radius, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapCircleAll(point, radius, layerMask, minDepth, maxDepth);
}

/*-----------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCollider: Returns all intersections of an overlapping Collider2D against the Collider2D in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapCollider(pCollider, contactFilter);
}

/*-----------------------------------------------------------------------------------------------------------------------------------
| --- OverlapCollider: Returns all intersections of an overlapping Collider2D against the Collider2D in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapCollider(const Collider2D* pCollider, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapCollider(pCollider, contactFilter, results);
}

/*---------------------------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Returns all intersections of an overlapping Point against the Collider2D in this Physics2DSubsystem --- |
---------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter) const
{
	return m_defaultScene.OverlapPoint(point, contactFilter);
}

/*---------------------------------------------------------------------------------------------------------------------------
| --- OverlapPoint: Returns all intersections of an overlapping Point against the Collider2D in this Physics2DSubsystem --- |
---------------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Physics2DSubsystem::OverlapPoint(const Vector2f& point, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
	return m_defaultScene.OverlapPoint(point, contactFilter, results);
}

/*-------------------------------------------------------------------------------------------------------------------------------
| --- OverlapPointAll: Returns all of the unique Collider2D intersected by the overlapping Point in this Physics2DSubsystem --- |
-------------------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::Collider2D*> CaledonEngine::Physics2DSubsystem::OverlapPointAll(const Vector2f& point, int layerMask, float minDepth, float maxDepth) const
{
	return m_defaultScene.OverlapPointAll(point, layerMask, minDepth, maxDepth);
}

/*---------------------------------------------------------------------------------------------------------------------------
| --- GetRayIntersection: Returns the nearest intersection of a Ray2D against the Collider2D in this Physics2DSubsystem --- |
---------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::ColliderHit2D CaledonEngine::Physics2DSubsystem::GetRayIntersection(const Ray2D& ray, float distance, int layerMask) const
{
	auto hits = m_defaultScene.GetRayIntersection(ray, distance, layerMask);
	return hits.empty() ? ColliderHit2D{} : hits.front();
}

/*-----------------------------------------------------------------------------------------------------------------------
| --- GetRayIntersectionAll: Returns all of the unique Collider2D intersected by a Ray2D in this Physics2DSubsystem --- |
-----------------------------------------------------------------------------------------------------------------------*/
std::vector<CaledonEngine::ColliderHit2D> CaledonEngine::Physics2DSubsystem::GetRayIntersectionAll(const Ray2D& ray, float distance, int layerMask) const
{
	return m_defaultScene.GetRayIntersectionAll(ray, distance, layerMask);
}

/*--------------------------------------------------------------------------------------------------------------------------
| --- ClosestPoint: Returns a point on the perimeter of the given Collider2D that is closest to the specified position --- |
--------------------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Physics2DSubsystem::ClosestPoint(const Vector2f& position, const Collider2D* pCollider) const
{
	return m_defaultScene.ClosestPoint(position, pCollider);
}

/*--------------------------------------------------------------------------
| --- Distance: Calculates the minimum distance between two Collider2D --- |
--------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::Physics2DSubsystem::Distance(const Collider2D* pColliderA, const Collider2D* pColliderB) const
{
	return m_defaultScene.Distance(pColliderA, pColliderB);
}

/*---------------------------------------------------------------------
| --- Simulate: Simulates physics in the default 2D physics scene --- |
---------------------------------------------------------------------*/
bool CaledonEngine::Physics2DSubsystem::Simulate(float deltaTime, int simulatedLayers)
{
	return m_defaultScene.Simulate(deltaTime, simulatedLayers);
}

/*---------------------------------------------------------------------------------
| --- SyncTransforms: Synchronizes transform positions with 2D physics bodies --- |
---------------------------------------------------------------------------------*/
void CaledonEngine::Physics2DSubsystem::SyncTransforms()
{
	m_defaultScene.SyncTransforms();
}