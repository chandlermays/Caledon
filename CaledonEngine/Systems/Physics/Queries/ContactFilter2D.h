/*------------------------------
| File: ColliderHit2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

// A set of parameters for filtering contact results. Define the angle by referring to their position in world space:
// 0 degrees is parallel the positive x-axis
// 90 degrees is parallel to the positive y-axis
// 180 degrees is parallel to the negative x-axis
// 270 degrees is parallel to the negative y-axis

// Use a contact filter to precisely control which contact results get returned. This removes the need to filter the results later, is faster, and more convenient.

namespace CaledonEngine
{
	struct ContactFilter2D
	{
//	static properties:
//		static ContactFilter2D noFilter									// Returns a new contact filter with a state where it will not filter any contacts
//		static float NormalAngleUpperLimit								// A constant of the maximum normal angle used of 359.9999f

//  properties:
//		bool isFiltering												// Given the current state of the contact filter, determine whether it would filter anything
//		LayerMask layerMask												// Sets the contact filter to filter the results that only include Collider2D on the layers defined by the layer mask
//		float maxDepth													// Sets the contact filter to filter the results to only include Collider2D with a Z coordinate(depth) less than this value
//		float maxNormalAngle											// Sets the contact filter to filter the results to only include contacts with collision normal angles that are less than this angle
//		float minDepth													// Sets the contact filter to filter the results to only include Collider2D with a Z coordinate(depth) greater than this value
//		float minNormalAngle											// Sets the contact filter to filter the results to only include contacts with collision normal angles that are greater than this angle
//		bool useDepth													// Sets the contact filter to filter the results by depth using minDepth and maxDepth
//		bool useLayerMask												// Sets the contact filter to filter results by layer mask
//		bool useNormalAngle												// Sets the contact filter to filter the results by the collision's normal angle using minNormalAngle and maxNormalAngle
//		bool useOutsideDepth											// Sets the contact filter to filter within the minDepth and maxDepth range, or outside that range
//		bool useOutsideNormalAngle										// Sets the contact filter to filter within the minNormalAngle and maxNormalAngle range, or outside that range
//		bool useTriggers												// Sets to filter contact results based on trigger collider involvement

//	public methods:
//		void ClearDepth													// Turns off depth filtering by setting useDepth to false.The associated values of minDepth and maxDepth are not changed
//		void ClearLayerMask												// Turns off layer mask filtering by setting useLayerMask to false.The associated value of layerMask is not changed
//		void ClearNormalAngle											// Turns off normal angle filtering by setting useNormalAngle to false.The associated values of minNormalAngle and maxNormalAngle are not changed
//		bool IsFilteringDepth(GameObject obj)							// Checks if the Transform for obj is within the depth range to be filtered
//		bool IsFilteringLayerMask(GameObject obj)						// Checks if the GameObject.layer for obj is included in the layerMask to be filtered
//		bool IsFilteringNormalAngle(Vector2 normal)						// Checks if the angle of normal is within the normal angle range to be filtered
//		bool IsFilteringTrigger(Collider2D collider)					// Checks if the collider is a trigger and should be filtered by the useTriggers to be filtered
//		void SetDepth(float min, float max)								// Sets the minDepth and maxDepth filter properties and turns on depth filtering by setting useDepth to true
//		void SetLayerMask(LayerMask layerMask)							// Sets the layerMask filter property using the layerMask parameter provided and also enables layer mask filtering by setting useLayerMask to true
//		void SetNormalAngle(float min, float max)						// Sets the minNormalAngle and maxNormalAngle filter properties and turns on normal angle filtering by setting useNormalAngle to true
	};
}