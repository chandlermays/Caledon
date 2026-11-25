/*------------------------------
| File: Transform.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Scene/Component.h"
#include "Utilities/Math/Vector2.h"

namespace CaledonEngine
{
	/*---------------------------------------------------------------------------------------------------
	| --- Transform: Stores the pose relative to the parent GameObject (local) and works out the    --- |
	| ---            world pose from the parents when it is asked for, caching it until something   --- |
	| ---            above it changes. GetPosition/SetPosition (and rotation, scale) are WORLD, so  --- |
	| ---            physics and rendering read the same values as before parenting existed         --- |
	| ---                                                                                           --- |
	| ---            World position is exact: it goes through the parents' full 2x2 matrices, so it --- |
	| ---            stays right even when a parent's non-uniform scale shears its children.        --- |
	| ---            World rotation is the parents' rotations added up (Unity composes rotations    --- |
	| ---            the same way). World scale is "lossy" (Unity's lossyScale): the per-axis       --- |
	| ---            product of the parents' scales, ignoring rotation, because a shear can't be    --- |
	| ---            held by position/rotation/scale. Rotation and scale are exact when every       --- |
	| ---            parent's scale is uniform in size (e.g. 2x2 or -1x1) or nothing is rotated.    --- |
	| ---            A parent with one negative scale axis mirrors its children, so their rotation  --- |
	| ---            turns the other way (worldRotation = parent - local)                           --- |
	---------------------------------------------------------------------------------------------------*/
	class Transform : public Component
	{
	private:
		Vector2f m_localPosition;																			// The position relative to the parent (world position when there is no parent)
		Vector2f m_localScale;																				// The scale relative to the parent
		float m_localRotation;																				// The rotation relative to the parent, in degrees

		mutable Vector2f m_worldPosition;																	// Cached world position (valid while m_isWorldDirty is false)
		mutable Vector2f m_worldScale;																		// Cached world (lossy) scale
		mutable float m_worldRotation;																		// Cached world rotation, in degrees
		mutable float m_worldLinear[4];																		// Cached world 2x2 matrix (rotation, scale and any shear) as { a, b, c, d } = [a c; b d]
		mutable bool m_isWorldDirty;																		// Flag to indicate the cached world pose must be recalculated

		const Transform* GetParentTransform() const;														// Returns the Transform of the owner's parent, or nullptr at the root
		void UpdateWorld() const;																			// Recalculates the cached world pose from the parent's world pose

	public:
		Transform();																						// Constructor
		Transform(const Vector2f& localPosition, float localRotation, const Vector2f& localScale);			// Parameterized Constructor (local values)
		~Transform() override = default;																	// Destructor
		Transform(const Transform&) = delete;																// Prevent copy-construction
		Transform& operator=(const Transform&) = delete;													// Prevent copy-assignment
		Transform(Transform&&) = delete;																	// Prevent move-construction
		Transform& operator=(Transform&&) = delete;															// Prevent move-assignment

		const std::string& GetTypeName() const override;													// Returns the type name of the Component

		const Vector2f& GetPosition() const;																// Returns the world position of the object
		void SetPosition(const Vector2f& position);															// Sets the world position of the object
		float GetRotation() const;																			// Returns the world rotation of the object, in degrees
		void SetRotation(float rotation);																	// Sets the world rotation of the object, in degrees
		const Vector2f& GetScale() const;																	// Returns the world (lossy) scale of the object
		void SetScale(const Vector2f& scale);																// Sets the world scale of the object

		const Vector2f& GetLocalPosition() const noexcept;													// Returns the position relative to the parent
		void SetLocalPosition(const Vector2f& localPosition);												// Sets the position relative to the parent
		float GetLocalRotation() const noexcept;															// Returns the rotation relative to the parent, in degrees
		void SetLocalRotation(float localRotation);															// Sets the rotation relative to the parent, in degrees
		const Vector2f& GetLocalScale() const noexcept;														// Returns the scale relative to the parent
		void SetLocalScale(const Vector2f& localScale);														// Sets the scale relative to the parent

		Vector2f TransformPoint(const Vector2f& localPoint) const;											// Converts a point from this object's local space to world space
		Vector2f InverseTransformPoint(const Vector2f& worldPoint) const;									// Converts a point from world space to this object's local space

		void MarkWorldDirty() const;																		// Flags the cached world pose (and every child's) to be recalculated
	};
}
