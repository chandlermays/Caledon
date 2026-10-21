/*------------------------------
| File: CompositeCollider2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "CompositeCollider2D.h"
#include "Systems/Physics/Shapes/PhysicsShapeGroup2D.h"
#include <algorithm>
#include <cmath>

namespace
{
	/*---------------------------------------------------------------------------------------
	| --- WeldVertices:
	---------------------------------------------------------------------------------------*/
	void WeldVertices(std::vector<CaledonEngine::Vector2f>& loop, float minDistance)
	{
		if (!(minDistance > 0.0f) || loop.size() < 2)
			return;

		const float minSq = minDistance * minDistance;
		auto closeTo = [minSq](const CaledonEngine::Vector2f& a, const CaledonEngine::Vector2f& b)
			{
				const float dx = a.x - b.x;
				const float dy = a.y - b.y;
				return dx * dx + dy * dy < minSq;
			};

		std::size_t kept = 1;
		for (std::size_t i = 1; i < loop.size(); ++i)
		{
			if (!closeTo(loop[i], loop[kept - 1]))
				loop[kept++] = loop[i];
		}

		while (kept > 1 && closeTo(loop[kept - 1], loop[0]))
			--kept;

		loop.resize(kept);
	}
}

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*------------------------------------------------------------------------------
| --- Constructor: Constructs this CompositeCollider2D with default values --- |
------------------------------------------------------------------------------*/
CaledonEngine::CompositeCollider2D::CompositeCollider2D()
	: m_edgeRadius{ 0.0f }
	, m_generationType{ GenerationType::Synchronous }
	, m_geometryType{ GeometryType::Outlines }
	, m_offsetDistance{ 0.00005f }
	, m_useDelaunayMesh{ false }
	, m_vertexDistance{ 0.0005f }
	, m_isGeometryDirty{ false }
{ }

/*--------------------------------------------------------------
| --- GetTypeName: Returns the type name of this Component --- |
--------------------------------------------------------------*/
const std::string& CaledonEngine::CompositeCollider2D::GetTypeName() const
{
	static const std::string typeName = "CompositeCollider2D";
	return typeName;
}

/*---------------------------------------------------------------------------------------
| --- SetEdgeRadius: 
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::SetEdgeRadius(float edgeRadius)
{
	edgeRadius = std::max(edgeRadius, 0.0f);
	if (edgeRadius == m_edgeRadius)
		return;

	m_edgeRadius = edgeRadius;
	OnChanged();
}

/*---------------------------------------------------------------------------------------
| --- SetGenerationType:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::SetGenerationType(GenerationType generationType)
{
	m_generationType = generationType;

	if (m_generationType == GenerationType::Synchronous && m_isGeometryDirty)
		GenerateGeometry();
}

/*---------------------------------------------------------------------------------------
| --- SetGeometryType:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::SetGeometryType(GeometryType geometryType)
{
	if (geometryType == m_geometryType)
		return;

	m_geometryType = geometryType;
	OnChanged();
}

/*---------------------------------------------------------------------------------------
| --- SetOffsetDistance:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::SetOffsetDistance(float offsetDistance)
{
	offsetDistance = std::max(offsetDistance, 0.0f);
	if (offsetDistance == m_offsetDistance)
		return;

	m_offsetDistance = offsetDistance;
	OnChanged();
}

/*---------------------------------------------------------------------------------------
| --- SetUseDelaunayMesh:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::SetUseDelaunayMesh(bool useDelaunayMesh)
{
	if (useDelaunayMesh == m_useDelaunayMesh)
		return;

	m_useDelaunayMesh = useDelaunayMesh;
	OnChanged();
}

/*---------------------------------------------------------------------------------------
| --- SetVertexDistance:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::SetVertexDistance(float vertexDistance)
{
	vertexDistance = std::max(vertexDistance, 0.0f);
	if (vertexDistance == m_vertexDistance)
		return;

	m_vertexDistance = vertexDistance;
	OnChanged();
}

/*---------------------------------------------------------------------------------------
| --- GenerateGeometry:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::GenerateGeometry()
{
	PhysicsShapeGroup2D shapes;

	for (Collider2D* pCollider : m_compositedColliders)
		pCollider->GetShapes(shapes);

	m_pathVertices.clear();
	m_paths.clear();

	MergeShapesIntoPaths(shapes);

	m_isGeometryDirty = false;
}

/*---------------------------------------------------------------------------------------
| --- GetCompositedColliders:
---------------------------------------------------------------------------------------*/
int CaledonEngine::CompositeCollider2D::GetCompositedColliders(std::vector<Collider2D*>& colliders) const
{
	colliders.assign(m_compositedColliders.begin(), m_compositedColliders.end());
	return static_cast<int>(colliders.size());
}

/*---------------------------------------------------------------------------------------
| --- GetPath:
---------------------------------------------------------------------------------------*/
std::span<const CaledonEngine::Vector2f> CaledonEngine::CompositeCollider2D::GetPath(int index) const noexcept
{
	if (index < 0 || index >= GetPathCount())
		return {};

	const PathRange& range = m_paths[static_cast<std::size_t>(index)];
	return std::span<const Vector2f>(m_pathVertices.data() + range.start, static_cast<std::size_t>(range.count));
}

/*---------------------------------------------------------------------------------------
| --- GetPath:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::GetPath(int index, std::vector<Vector2f>& points) const
{
	const std::span<const Vector2f> path = GetPath(index);
	points.assign(path.begin(), path.end());
}

/*---------------------------------------------------------------------------------------
| --- GetPathPointCount:
---------------------------------------------------------------------------------------*/
int CaledonEngine::CompositeCollider2D::GetPathPointCount(int index) const noexcept
{
	return static_cast<int>(GetPath(index).size());
}

/*---------------------------------------------------------------------------------------
| --- AddCompositeCollider:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::CompositeCollider2D::AddCompositedCollider(Collider2D* pCollider)
{
	if (!pCollider || pCollider == this)
		return false;

	if (std::find(m_compositedColliders.begin(), m_compositedColliders.end(), pCollider) != m_compositedColliders.end())
		return false;

	m_compositedColliders.push_back(pCollider);
	OnChanged();
	return true;
}

/*---------------------------------------------------------------------------------------
| --- RemoveCompositedCollider:
---------------------------------------------------------------------------------------*/
bool CaledonEngine::CompositeCollider2D::RemoveCompositedCollider(Collider2D* pCollider)
{
	const auto it = std::find(m_compositedColliders.begin(), m_compositedColliders.end(), pCollider);
	if (it == m_compositedColliders.end())
		return false;

	m_compositedColliders.erase(it);
	OnChanged();
	return true;
}

/*---------------------------------------------------------------------------------------
| --- AddCompositedColliders:
---------------------------------------------------------------------------------------*/
int CaledonEngine::CompositeCollider2D::AddCompositedColliders(std::span<Collider2D* const> colliders)
{
	int added = 0;
	m_compositedColliders.reserve(m_compositedColliders.size() + colliders.size());

	for (Collider2D* pCollider : colliders)
	{
		if (!pCollider || pCollider == this)
			continue;

		if (std::find(m_compositedColliders.begin(), m_compositedColliders.end(), pCollider) != m_compositedColliders.end())
			continue;

		m_compositedColliders.push_back(pCollider);
		++added;
	}

	if (added > 0)
		OnChanged();

	return added;
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*---------------------------------------------------------------------------------------
| --- OnChanged:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::OnChanged()
{
	m_isGeometryDirty = true;

	if (m_generationType == GenerationType::Synchronous)
		GenerateGeometry();
}

/*---------------------------------------------------------------------------------------
| --- MergeShapesIntoPaths:
---------------------------------------------------------------------------------------*/
void CaledonEngine::CompositeCollider2D::MergeShapesIntoPaths(const PhysicsShapeGroup2D& shapes)
{
	std::vector<Vector2f> loop;

	for (int i = 0; i < shapes.GetShapeCount(); ++i)
	{
		if (shapes.GetShape(i)->shapeType != PhysicsShapeType2D::Polygon)
			continue;

		const std::span<const Vector2f> vertices = shapes.GetShapeVertices(i);
		loop.assign(vertices.begin(), vertices.end());

		WeldVertices(loop, m_vertexDistance);
		if (loop.size() < 3)
			continue;

		m_paths.push_back(PathRange{ static_cast<int>(m_pathVertices.size()), static_cast<int>(loop.size()) });
		m_pathVertices.insert(m_pathVertices.end(), loop.begin(), loop.end());
	}
}