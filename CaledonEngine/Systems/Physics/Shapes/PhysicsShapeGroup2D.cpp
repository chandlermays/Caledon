/*------------------------------
| File: PhysicsShapeGroup2D.cpp
| Author: Chandler Mays
------------------------------*/
#include "PhysicsShapeGroup2D.h"

#include "Utilities/Math/Constants.h"

#include <cmath>

/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-----------------------------------------------------------------------------
| --- Constructor: Constructs the PhysicsShapeGroup2D with default values --- |
-----------------------------------------------------------------------------*/
CaledonEngine::PhysicsShapeGroup2D::PhysicsShapeGroup2D(int shapeCapacity, int vertexCapacity)
{
	if (shapeCapacity > 0)
		m_shapes.reserve(static_cast<std::size_t>(shapeCapacity));

	if (vertexCapacity > 0)
		m_vertices.reserve(static_cast<std::size_t>(vertexCapacity));
}

/*--------------------------------------------------------------------------------------------------------
| --- Add: Adds the shapes and vertices from another PhysicsShapeGroup2D to this PhysicsShapeGroup2D --- |
--------------------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsShapeGroup2D::Add(const PhysicsShapeGroup2D& physicsShapeGroup)
{
	if (&physicsShapeGroup == this)
	{
		const PhysicsShapeGroup2D copy(*this);
		Add(copy);
		return;
	}

	const int vertexOffset = GetVertexCount();

	m_shapes.reserve(m_shapes.size() + physicsShapeGroup.m_shapes.size());
	m_vertices.reserve(m_vertices.size() + physicsShapeGroup.m_vertices.size());

	m_vertices.insert(m_vertices.end(), physicsShapeGroup.m_vertices.begin(), physicsShapeGroup.m_vertices.end());

	for (PhysicsShape2D shape : physicsShapeGroup.m_shapes)
	{
		shape.vertexStartIndex += vertexOffset;
		m_shapes.push_back(shape);
	}
}

/*--------------------------------------------------------------
| --- AddBox: Adds a box shape to this PhysicsShapeGroup2D --- |
--------------------------------------------------------------*/
int CaledonEngine::PhysicsShapeGroup2D::AddBox(const Vector2f& center, const Vector2f& size, float angle, float edgeRadius)
{
	if (size.x < 0.0f || size.y < 0.0f || edgeRadius <= 0.0f)
		return -1;

	const float radians = angle * Math::Deg2Rad;
	const float c = std::cos(radians);
	const float s = std::sin(radians);
	const float hx = size.x * 0.5f;
	const float hy = size.y * 0.5f;

	const float local[4][2] = { { -hx, -hy }, { hx, -hy }, { hx, hy }, { -hx, hy } };
	Vector2f corners[4];

	for (int i = 0; i < 4; ++i)
	{
		corners[i] = Vector2f(center.x + local[i][0] * c - local[i][1] * s, center.y + local[i][0] * s + local[i][1] * c);
	}

	return AddShape(PhysicsShapeType2D::Polygon, corners, edgeRadius);
}

/*----------------------------------------------------------------------
| --- AddCapsule: Adds a capsule shape to this PhysicsShapeGroup2D --- |
----------------------------------------------------------------------*/
int CaledonEngine::PhysicsShapeGroup2D::AddCapsule(const Vector2f& vertex0, const Vector2f& vertex1, float radius)
{
	if (radius <= 0.0f)
		return -1;

	const Vector2f ends[2] = { vertex0, vertex1 };
	return AddShape(PhysicsShapeType2D::Capsule, ends, radius);
}

/*--------------------------------------------------------------------
| --- AddCircle: Adds a circle shape to this PhysicsShapeGroup2D --- |
--------------------------------------------------------------------*/
int CaledonEngine::PhysicsShapeGroup2D::AddCircle(const Vector2f& center, float radius)
{
	if (radius <= 0.0f)
		return -1;

	return AddShape(PhysicsShapeType2D::Circle, std::span<const Vector2f>(&center, 1), radius);
}

/*-------------------------------------------------------------------
| --- AddEdges: Adds an edges shape to this PhysicsShapeGroup2D --- |
-------------------------------------------------------------------*/
int CaledonEngine::PhysicsShapeGroup2D::AddEdges(std::span<const Vector2f> vertices, float edgeRadius)
{
	if (vertices.size() < 2 || edgeRadius <= 0.0f)
		return -1;

	return AddShape(PhysicsShapeType2D::Edges, vertices, edgeRadius);
}

/*----------------------------------------------------------------------
| --- AddPolygon: Adds a polygon shape to this PhysicsShapeGroup2D --- |
----------------------------------------------------------------------*/
int CaledonEngine::PhysicsShapeGroup2D::AddPolygon(std::span<const Vector2f> vertices)
{
	if (vertices.size() < 3)
		return -1;

	return AddShape(PhysicsShapeType2D::Polygon, vertices, 0.0f);
}

/*-----------------------------------------------------------------------------
| --- Clear: Clears all shapes and vertices from this PhysicsShapeGroup2D --- |
-----------------------------------------------------------------------------*/
void CaledonEngine::PhysicsShapeGroup2D::Clear() noexcept
{
	m_shapes.clear();
	m_vertices.clear();
}

/*--------------------------------------------------------------------
| --- DeleteShape: Deletes a shape from this PhysicsShapeGroup2D --- |
--------------------------------------------------------------------*/
bool CaledonEngine::PhysicsShapeGroup2D::DeleteShape(int shapeIndex)
{
	if (!IsValidShape(shapeIndex))
		return false;

	const PhysicsShape2D removed = m_shapes[static_cast<std::size_t>(shapeIndex)];

	const auto firstVertex = m_vertices.begin() + removed.vertexStartIndex;
	m_vertices.erase(firstVertex, firstVertex + removed.vertexCount);
	m_shapes.erase(m_shapes.begin() + shapeIndex);

	for (std::size_t i = static_cast<std::size_t>(shapeIndex); i < m_shapes.size(); ++i)
	{
		m_shapes[i].vertexStartIndex -= removed.vertexCount;
	}

	return true;
}

/*------------------------------------------------------------------------------
| --- GetShape: Returns a pointer to the PhysicsShape2D at the given index --- |
------------------------------------------------------------------------------*/
const CaledonEngine::PhysicsShape2D* CaledonEngine::PhysicsShapeGroup2D::GetShape(int shapeIndex) const noexcept
{
	return IsValidShape(shapeIndex) ? &m_shapes[static_cast<std::size_t>(shapeIndex)] : nullptr;
}

/*--------------------------------------------------------------------------------------------------------------
| --- GetShapeData: Copies the shapes and vertices from this PhysicsShapeGroup2D into the provided vectors --- |
--------------------------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsShapeGroup2D::GetShapeData(std::vector<PhysicsShape2D>& shapes, std::vector<Vector2f>& vertices) const
{
	shapes.assign(m_shapes.begin(), m_shapes.end());
	vertices.assign(m_vertices.begin(), m_vertices.end());
}

/*-------------------------------------------------------------------------------------------------------------
| --- GetShapeVertex: Returns a pointer to the vertex at the given index for the shape at the given index --- |
-------------------------------------------------------------------------------------------------------------*/
const CaledonEngine::Vector2f* CaledonEngine::PhysicsShapeGroup2D::GetShapeVertex(int shapeIndex, int vertexIndex) const noexcept
{
	const PhysicsShape2D* pShape = GetShape(shapeIndex);
	if (!pShape || vertexIndex < 0 || vertexIndex >= pShape->vertexCount)
		return nullptr;

	return &m_vertices[static_cast<std::size_t>(pShape->vertexStartIndex + vertexIndex)];
}

/*-------------------------------------------------------------------------------------------
| --- GetShapeVertices: Returns a span of the vertices for the shape at the given index --- |
-------------------------------------------------------------------------------------------*/
std::span<const CaledonEngine::Vector2f> CaledonEngine::PhysicsShapeGroup2D::GetShapeVertices(int shapeIndex) const noexcept
{
	const PhysicsShape2D* pShape = GetShape(shapeIndex);
	if (!pShape)
		return {};

	return std::span<const Vector2f>(m_vertices.data() + pShape->vertexStartIndex, static_cast<std::size_t>(pShape->vertexCount));
}

/*---------------------------------------------------------------------------------------------------------
| --- GetShapeVertices: Copies the vertices for the shape at the given index into the provided vector --- |
---------------------------------------------------------------------------------------------------------*/
void CaledonEngine::PhysicsShapeGroup2D::GetShapeVertices(int shapeIndex, std::vector<Vector2f>& vertices) const
{
	const std::span<const Vector2f> view = GetShapeVertices(shapeIndex);
	vertices.assign(view.begin(), view.end());
}

/*-----------------------------------------------------------------------------------------------
| --- SetShapeAdjacentVertices: Sets the adjacent vertices for the shape at the given index --- |
-----------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsShapeGroup2D::SetShapeAdjacentVertices(int shapeIndex, bool useAdjStart, bool useAdjEnd, const Vector2f& adjStart, const Vector2f& adjEnd)
{
	if (!IsValidShape(shapeIndex))
		return false;

	PhysicsShape2D& shape = m_shapes[static_cast<std::size_t>(shapeIndex)];
	if (shape.shapeType != PhysicsShapeType2D::Edges)
		return false;

	shape.useAdjacentStart = useAdjStart;
	shape.useAdjacentEnd = useAdjEnd;
	shape.adjacentStart = adjStart;
	shape.adjacentEnd = adjEnd;
	return true;
}

/*--------------------------------------------------------------------------
| --- SetShapeRadius: Sets the radius for the shape at the given index --- |
--------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsShapeGroup2D::SetShapeRadius(int shapeIndex, float radius)
{
	if (!IsValidShape(shapeIndex) || radius <= 0.0f)
		return false;

	m_shapes[static_cast<std::size_t>(shapeIndex)].radius = radius;
	return true;
}

/*---------------------------------------------------------------------------------------------
| --- SetShapeVertex: Sets the vertex at the given index for the shape at the given index --- |
---------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsShapeGroup2D::SetShapeVertex(int shapeIndex, int vertexIndex, const Vector2f& vertex)
{
	const PhysicsShape2D* pShape = GetShape(shapeIndex);
	if (!pShape || vertexIndex < 0 || vertexIndex >= pShape->vertexCount)
		return false;

	m_vertices[static_cast<std::size_t>(pShape->vertexStartIndex + vertexIndex)] = vertex;
	return true;
}



/*------------------------------------
| --- Private Method Definitions --- |
------------------------------------*/
/*------------------------------------------------------------
| --- AddShape: Adds a shape to this PhysicsShapeGroup2D --- |
------------------------------------------------------------*/
int CaledonEngine::PhysicsShapeGroup2D::AddShape(PhysicsShapeType2D type, std::span<const Vector2f> vertices, float radius)
{
	m_shapes.reserve(m_shapes.size() + 1);

	PhysicsShape2D shape;
	shape.shapeType = type;
	shape.radius = radius;
	shape.vertexStartIndex = GetVertexCount();
	shape.vertexCount = static_cast<int>(vertices.size());

	m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
	m_shapes.push_back(shape);

	return GetShapeCount() - 1;
}

/*---------------------------------------------------------------------------------------------
| --- IsValidShape: Returns true if the shape index is valid for this PhysicsShapeGroup2D --- |
---------------------------------------------------------------------------------------------*/
bool CaledonEngine::PhysicsShapeGroup2D::IsValidShape(int shapeIndex) const noexcept
{
	return shapeIndex >= 0 && shapeIndex < GetShapeCount();
}