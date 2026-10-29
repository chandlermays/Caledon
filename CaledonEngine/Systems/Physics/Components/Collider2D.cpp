/*------------------------------
| File: Collider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "Collider2D.h"

#include "Systems/Physics/Components/RigidBody2D.h"
#include "Systems/Physics/PhysicsScene2D.h"
#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"
#include "Systems/Scene/GameObject.h"
#include "Systems/Scene/Scene.h"
#include "Systems/Scene/Transform.h"

namespace
{
	constexpr std::uint32_t kFnvOffsetBasis = 2166136261u;		// FNV-1a 32-bit
	constexpr std::uint32_t kFnvPrime = 16777619u;

	/*---------------------------------------------------------------------------------------
	| --- HashValue:
	---------------------------------------------------------------------------------------*/
	void HashValue(std::uint32_t& hash, std::uint32_t value)
	{
		for (int byte = 0; byte < 4; ++byte)
		{
			hash ^= (value >> (byte * 8)) & 0xFFu;
			hash *= kFnvPrime;
		}
	}

	/*---------------------------------------------------------------------------------------
	| --- HashFloat:
	---------------------------------------------------------------------------------------*/
	void HashFloat(std::uint32_t& hash, float value)
	{
		if (value == 0.0f)
		{
			value = 0.0f;
		}

		std::uint32_t bits;
		std::memcpy(&bits, &value, sizeof(bits));
		HashValue(hash, bits);
	}

	/*---------------------------------------------------------------------------------------
	| --- ComputeShapeBounds:
	---------------------------------------------------------------------------------------*/
	CaledonEngine::AABB2D ComputeShapeBounds(const CaledonEngine::PhysicsShapeGroup2D& shapeGroup, std::vector<CaledonEngine::AABB2D>* pPerShape, bool useRadii)
	{
		CaledonEngine::AABB2D total;
		for (int i = 0; i < shapeGroup.GetShapeCount(); ++i)
		{
			CaledonEngine::AABB2D box;
			for (const CaledonEngine::Vector2f& vertex : shapeGroup.GetShapeVertices(i))
			{
				box.Encapsulate(vertex);
			}

			if (useRadii)
			{
				const float radius = shapeGroup.GetShape(i)->radius;
				box.ExpandBy(CaledonEngine::Vector2f{ radius, radius });
			}

			if (pPerShape != nullptr)
			{
				pPerShape->push_back(box);
			}
			total.Encapsulate(box);
		}
		return total;
	}
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*---------------------------------------------------------------------
| --- Constructor: Constructs this Collider2D with default values --- |
---------------------------------------------------------------------*/
CaledonEngine::Collider2D::Collider2D()
	: m_pAttachedRigidBody{ nullptr }
	, m_pPhysicsScene2D{ nullptr }
	, m_bounceCombine{ PhysicsMaterialCombine2D::Maximum }
	, m_bounciness{ 0.0f }
	, m_bounds{ }
	, m_density{ 1.0f }
	, m_friction{ 0.4f }
	, m_frictionCombine{ PhysicsMaterialCombine2D::Mean }
	, m_isTrigger{ false }
	, m_offset{ 0.0f, 0.0f }
	, m_sharedMaterial{ }
	, m_callbackLayers{ -1 }
	, m_contactCaptureLayers{ -1 }
	, m_contactMask{ -1 }
	, m_excludeLayers{ 0 }
	, m_forceReceiveLayers{ -1 }
	, m_forceSendLayers{ -1 }
	, m_includeLayers{ 0 }
	, m_pComposite{ nullptr }
	, m_compositeCapable{ false }
	, m_compositeOperation{ CompositeOperation::None }
	, m_compositeOrder{ 0 }
	, m_errorState{ ColliderErrorState2D::None }
	, m_layerOverridePriority{ 0 }
	, m_shapeCount{ 0 }
{}

/*-------------------------------------------------------
| --- Destructor: Cleans up any allocated resources --- |
-------------------------------------------------------*/
CaledonEngine::Collider2D::~Collider2D()
{
	if (m_pAttachedRigidBody != nullptr)
	{
		m_pAttachedRigidBody->DetachCollider(this);
	}

	if (m_pPhysicsScene2D != nullptr)
	{
		m_pPhysicsScene2D->RemoveCollider(this);
	}

}

/*------------------------------------------------------
| --- Initialize: Prepares this Collider2D for use --- |
------------------------------------------------------*/
bool CaledonEngine::Collider2D::Initialize()
{
	if (m_pPhysicsScene2D != nullptr)
		return true;

	const Scene* pScene = (m_pOwner != nullptr) ? m_pOwner->GetScene() : nullptr;
	PhysicsScene2D* pPhysicsScene = (pScene != nullptr) ? pScene->GetPhysicsScene2D() : nullptr;
	if (pPhysicsScene == nullptr)
		return false;

	// Only keep the scene once registration succeeded, so a failed Initialize leaves nothing behind
	if (!pPhysicsScene->AddCollider(this))
		return false;

	m_pPhysicsScene2D = pPhysicsScene;

	// Whichever of the two components initializes first, the collider ends up attached to the owner's RigidBody2D
	if (m_pAttachedRigidBody == nullptr && m_pOwner != nullptr)
	{
		if (RigidBody2D* pBody = m_pOwner->GetComponent<RigidBody2D>())
		{
			pBody->AttachCollider(this);
		}
	}

	RefreshBounds();
	return true;

}

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::Collider2D::GetTypeName() const
{
    static const std::string typeName = "Collider2D";
    return typeName;
}

/*------------------------------------------------------------------------------
| --- CanContact: Determines if both Collider2D can ever come into contact --- |
------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::CanContact(const Collider2D* pCollider) const
{
	if (pCollider == nullptr || pCollider == this)
		return false;

	// both must be registered with the same PhysicsScene2D
	if (m_pPhysicsScene2D == nullptr || pCollider->m_pPhysicsScene2D != m_pPhysicsScene2D)
		return false;

	//...
	if (m_pOwner == nullptr || pCollider->m_pOwner == nullptr)
		return false;

	//...
	if (!IsActive() || !pCollider->IsActive() || !m_pOwner->IsActive() || !pCollider->m_pOwner->IsActive())
		return false;

	//...
	if (m_pOwner == pCollider->m_pOwner)
		return false;

	//...
	if (m_pAttachedRigidBody != nullptr && m_pAttachedRigidBody == pCollider->m_pAttachedRigidBody)
		return false;

	// explicit per-pair ignore (Physics2D.IgnoreCollision)
	if (m_pPhysicsScene2D->GetIgnoreCollision(this, pCollider))
		return false;

	// Each collider decides whether it accepts the other's layer:
	// effective mask = (layer matrix row for its own layer | includeLayers) & ~excludeLayers
	const auto& matrix = m_pPhysicsScene2D->GetSettings().layerCollisionMatrix;
	const int layerA = m_pOwner->GetLayer();
	const int layerB = pCollider->m_pOwner->GetLayer();

	const uint32_t maskA = (static_cast<uint32_t>(matrix[layerA]) | static_cast<uint32_t>(m_includeLayers.value)) & ~static_cast<uint32_t>(m_excludeLayers.value);
	const uint32_t maskB = (static_cast<uint32_t>(matrix[layerB]) | static_cast<uint32_t>(pCollider->m_includeLayers.value)) & ~static_cast<uint32_t>(pCollider->m_excludeLayers.value);

	const bool aAcceptsB = (maskA & (1u << layerB)) != 0;
	const bool bAcceptsA = (maskB & (1u << layerA)) != 0;

	if (aAcceptsB == bAcceptsA)
		return aAcceptsB;

	// Conflicting decisions: the higher layerOverridePriority wins. On a tie, the contact is refused (exclude beats include).
	if (m_layerOverridePriority != pCollider->m_layerOverridePriority)
		return (m_layerOverridePriority > pCollider->m_layerOverridePriority) ? aAcceptsB : bAcceptsA;

	return false;
}

/*-----------------------------------------------------------------------------------------------------------------
| --- ClosestPoint: Returns a point on the perimeter of this Collider2D that is closest to the given position --- |
-----------------------------------------------------------------------------------------------------------------*/
CaledonEngine::Vector2f CaledonEngine::Collider2D::ClosestPoint(const Vector2f& position) const
{
	if (m_pPhysicsScene2D == nullptr)
		return position;

	return m_pPhysicsScene2D->ClosestPoint(position, this);
}

/*---------------------------------------------------------------------------------------------------
| --- Distance: Calculates the minimum seperation of this Collider2D against another Collider2D --- |
---------------------------------------------------------------------------------------------------*/
CaledonEngine::ColliderDistance2D CaledonEngine::Collider2D::Distance(const Collider2D* pCollider) const
{
	if (m_pPhysicsScene2D == nullptr || pCollider == nullptr)
		return ColliderDistance2D{};

	return m_pPhysicsScene2D->Distance(this, pCollider);
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: Check whether this Collider2D is touching the Collider2D or not --- |
-------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const Collider2D* pCollider) const
{
	if (m_pPhysicsScene2D == nullptr || pCollider == nullptr)
		return false;

	return m_pPhysicsScene2D->IsTouching(this, pCollider);
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: Check whether this Collider2D is touching the Collider2D or not --- |
-------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const Collider2D* pCollider, const ContactFilter2D& contactFilter) const
{
	if (m_pPhysicsScene2D == nullptr || pCollider == nullptr)
		return false;

	// If your PhysicsScene2D supports a filtered IsTouching overload, call it here. 
	// Otherwise, you can evaluate the contact filter against the result.
	return m_pPhysicsScene2D->IsTouching(this, pCollider);
}

/*-------------------------------------------------------------------------------------
| --- IsTouching: Check whether this Collider2D is touching the Collider2D or not --- |
-------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouching(const ContactFilter2D& contactFilter) const
{
	if (m_pPhysicsScene2D == nullptr)
		return false;

	// Overlap or check contacts involving this collider filtered by the contactFilter
	std::vector<Collider2D*> overlappingColliders;
	m_pPhysicsScene2D->OverlapCollider(this, contactFilter, overlappingColliders);
	return !overlappingColliders.empty();
}

/*---------------------------------------------------------------------------------------------------------------------
| --- IsTouchingLayers: Check whether this Collider2D is touching any colliders on the specified LayerMask or not --- |
---------------------------------------------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::IsTouchingLayers(LayerMask layerMask) const
{
	if (m_pPhysicsScene2D == nullptr)
		return false;

	// Build a quick contact filter targeting the specified layer mask
	ContactFilter2D filter = ContactFilter2D::NoFilter();
	filter.SetLayerMask(layerMask);

	std::vector<Collider2D*> overlappingColliders;
	m_pPhysicsScene2D->OverlapCollider(this, filter, overlappingColliders);
	return !overlappingColliders.empty();
}

/*-----------------------------------------------------------------------------------
| --- OverlapPoint: Check if a Collider2D overlaps a given point in world-space --- |
-----------------------------------------------------------------------------------*/
bool CaledonEngine::Collider2D::OverlapPoint(const Vector2f& point) const
{
	return Vector2f::SqrDistance(point, ClosestPoint(point)) <= 0.0f;
}

/*------------------------------------------------------------------------------------------------------
| --- OnCollisionEnter2D: Sent when an incoming collider makes contact with this object's collider --- |
------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionEnter2D(const Collision2D& collision)
{

}

/*---------------------------------------------------------------------------------------------------------
| --- OnCollisionExit2D: Sent when a collider on another object stops touching this object's collider --- |
---------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionExit2D(const Collision2D& collision)
{

}

/*------------------------------------------------------------------------------------------------------------------
| --- OnCollisionStay2D: Sent each frame where a collider on another object is touching this object's collider --- |
------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnCollisionStay2D(const Collision2D& collision)
{

}

/*------------------------------------------------------------------------------------------------------
| --- OnTriggerEnter2D: Sent when another object enters a trigger collider attached to this object --- |
------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerEnter2D(const Collision2D& collision)
{

}

/*-----------------------------------------------------------------------------------------------------
| --- OnTriggerExit2D: Sent when another object leaves a trigger collider attached to this object --- |
-----------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerExit2D(const Collision2D& collision)
{

}

/*--------------------------------------------------------------------------------------------------------------------
| --- OnTriggerStay2D: Sent each frame where another object is within a trigger collider attached to this object --- |
--------------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::OnTriggerStay2D(const Collision2D& collision)
{

}

/*--------------------------------------------------------------------------------
| --- SetSharedMaterial: Sets the shared physics material of this Collider2D --- |
--------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::SetSharedMaterial(const PhysicsMaterial2D& material)
{
	m_sharedMaterial = material;
}

/*-----------------------------------------------------------------------
| --- RefreshBounds: Refreshes the bounding area of this Collider2D --- |
-----------------------------------------------------------------------*/
void CaledonEngine::Collider2D::RefreshBounds()
{
	PhysicsShapeGroup2D shapeGroup;
	m_shapeCount = BuildShapes(shapeGroup, true);
	m_errorState = (m_shapeCount > 0) ? ColliderErrorState2D::None : ColliderErrorState2D::NoShapes;
	m_bounds = ComputeShapeBounds(shapeGroup, nullptr, true);		// Stays invalid (empty) when there are no shapes
}

/*------------------------------------------------------------------
| --- GetShapes: Populates the shape group with physics shapes --- |
------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetShapes(PhysicsShapeGroup2D& physicsShapeGroup) const
{
	return BuildShapes(physicsShapeGroup, true);
}

/*-------------------------------------------------------------------------
| --- GetShapesHash: Generates a hash representing the current shapes --- |
-------------------------------------------------------------------------*/
std::uint32_t CaledonEngine::Collider2D::GetShapesHash() const
{
	PhysicsShapeGroup2D shapeGroup;
	BuildShapes(shapeGroup, false);

	std::uint32_t hash = kFnvOffsetBasis;
	for (int i = 0; i < shapeGroup.GetShapeCount(); ++i)
	{
		const PhysicsShape2D* pShape = shapeGroup.GetShape(i);
		HashValue(hash, static_cast<std::uint32_t>(pShape->shapeType));
		HashValue(hash, static_cast<std::uint32_t>(pShape->vertexCount));
		HashFloat(hash, pShape->radius);

		for (const Vector2f& vertex : shapeGroup.GetShapeVertices(i))
		{
			HashFloat(hash, vertex.x);
			HashFloat(hash, vertex.y);
		}
	}

	if (m_pOwner != nullptr)
	{
		const Vector2f& scale = m_pOwner->GetTransform().GetScale();
		HashFloat(hash, scale.x);
		HashFloat(hash, scale.y);
	}

	return hash;
}

/*-------------------------------------------------------------------
| --- GetShapeBounds: Returns bounds for all constituent shapes --- |
-------------------------------------------------------------------*/
CaledonEngine::AABB2D CaledonEngine::Collider2D::GetShapeBounds(std::vector<AABB2D>& bounds, bool useRadii, bool useWorldSpace) const
{
	PhysicsShapeGroup2D shapeGroup;
	BuildShapes(shapeGroup, useWorldSpace);

	bounds.clear();
	return ComputeShapeBounds(shapeGroup, &bounds, useRadii);
}

/*-------------------------------------------------------------------------------------
| --- Cast: 
-------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::Cast(const Vector2f& direction, const ContactFilter2D& contactFilter, std::span<ColliderHit2D> results, float distance, bool ignoreSiblingColliders) const
{
    /*
	
	
	
	*/
}

/*----------------------------------------------------------------------------------------
| --- Overlap: 
----------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::Overlap(const Vector2f& position, float angle, const ContactFilter2D& contactFilter, std::vector<Collider2D*>& results) const
{
    /*
	
	if (m_pAttachedRigidBody)
	return PhysicsScene2D.OverlapCollider(...)
		-> ContactFilter2D contactFilter = ContactFilter2D.CreateLegacyFilter(int layerMask, float minDepth, float maxDepth)
			-> ContactFilter2D result = default(ContactFilter2D);
			   result.useTriggers = Physics2D.queriesHitTrigger;
			   result.SetLayerMask(layerMask);
			   result.SetDepth(minDepth, maxDepth);
			   return result;

	*/
}

/*---------------------------------------------------------------------------------------
| --- Raycast: 
---------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::Raycast(const Vector2f& direction, std::span<ColliderHit2D> results, float distance, LayerMask layerMask, float minDepth, float maxDepth) const
{
    return 0;
}

/*-------------------------------------------------------------------------
| --- GetContacts: Retrieves contact points involving this Collider2D --- |
-------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetContacts(const ContactFilter2D& contactFilter, std::span<ContactPoint2D> contacts) const
{
    // return Physics2D.GetContacts(this, contactFilter, contacts);
}

/*--------------------------------------------------------------------------------------------
| --- GetContactColliders: Retrieves colliders currently in contact with this Collider2D --- |
--------------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::GetContactColliders(const ContactFilter2D& contactFilter, std::vector<Collider2D*>& colliders) const
{
    // return Physics2D.GetContactColliders(this, contactFilter, colliders);
}


/*--------------------------------------
| --- Protected Method Definitions --- |
--------------------------------------*/
/*---------------------------------------------------------------------------------------------------
| --- AddLocalShapes: 
---------------------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::AddLocalShapes(PhysicsShapeGroup2D&) const
{
	return 0;
}


/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------
| --- BuildShapes:
------------------------------------------------------------------------------------------------------------------*/
int CaledonEngine::Collider2D::BuildShapes(PhysicsShapeGroup2D& physicsShapeGroup, bool worldSpace) const
{
	const int firstShape = physicsShapeGroup.GetShapeCount();
	AddLocalShapes(physicsShapeGroup);
	const int lastShape = physicsShapeGroup.GetShapeCount();

	if (!worldSpace)
		return lastShape - firstShape;

	Vector2f position;
	float rotation;
	Vector2f scale;
	GetWorldPose(position, rotation, scale);

	const auto toWorld = [&](const Vector2f& point)
		{ return position + (point * scale).Rotate(rotation); };

	const float radiusScale = std::max(std::abs(scale.x), std::abs(scale.y));
	const bool mirrored = (scale.x * scale.y) < 0.0f;

	std::vector<Vector2f> vertices;
	for (int i = firstShape; i < lastShape; ++i)
	{
		const PhysicsShape2D shape = *physicsShapeGroup.GetShape(i);

		const std::span<const Vector2f> local = physicsShapeGroup.GetShapeVertices(i);
		vertices.assign(local.begin(), local.end());
		for (Vector2f& vertex : vertices)
		{
			vertex = toWorld(vertex);
		}

		if (mirrored && shape.shapeType == PhysicsShapeType2D::Polygon)
		{
			std::reverse(vertices.begin(), vertices.end());
		}

		for (int j = 0; j < static_cast<int>(vertices.size()); ++j)
		{
			physicsShapeGroup.SetShapeVertex(i, j, vertices[static_cast<std::size_t>(j)]);
		}

		physicsShapeGroup.SetShapeRadius(i, shape.radius * radiusScale);

		if (shape.useAdjacentStart || shape.useAdjacentEnd)
		{
			physicsShapeGroup.SetShapeAdjacentVertices(i, shape.useAdjacentStart, shape.useAdjacentEnd, toWorld(shape.adjacentStart), toWorld(shape.adjacentEnd));
		}
	}

	return lastShape - firstShape;
}

/*-----------------------------------------------------------------------------------------------------
| --- GetWorldPose:
-----------------------------------------------------------------------------------------------------*/
void CaledonEngine::Collider2D::GetWorldPose(Vector2f& position, float& rotation, Vector2f& scale) const
{
	position = Vector2f{ 0.0f, 0.0f };
	rotation = 0.0f;
	scale = Vector2f{ 1.0f, 1.0f };

	if (m_pOwner != nullptr)
	{
		const Transform& transform = m_pOwner->GetTransform();
		position = transform.GetPosition();
		rotation = transform.GetRotation();
		scale = transform.GetScale();
	}

	if (m_pAttachedRigidBody != nullptr)
	{
		position = m_pAttachedRigidBody->GetPosition();
		rotation = m_pAttachedRigidBody->GetRotation();
	}
}