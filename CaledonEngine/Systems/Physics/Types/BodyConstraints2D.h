/*------------------------------
| File: BodyConstraints2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>

// Flags used to constrain the motion of a RigidBody2D.
// This is a bitmask enum - combine flags with bitwise OR (e.g. BodyConstraints2D::FreezePositionX | BodyConstraints2D::FreezeRotation)
// rather than assigning a single value.

namespace CaledonEngine
{
	enum class BodyConstraints2D : uint8_t
	{
		None = 0,														// No constraints
		FreezePositionX = 1 << 0,										// Freeze motion along the X-axis
		FreezePositionY = 1 << 1,										// Freeze motion along the Y-axis
		FreezeRotation = 1 << 2,										// Freeze rotation along the Z-axis
		FreezePosition = FreezePositionX | FreezePositionY,				// Freeze motion along the X-axis and Y-axis
		FreezeAll = FreezePosition | FreezeRotation						// Freeze rotation and motion along all axes
	};

	/*---------------------------------------------------------
	| --- operator|: Combines two BodyConstraints2D flags --- |
	---------------------------------------------------------*/
	constexpr BodyConstraints2D operator|(BodyConstraints2D lhs, BodyConstraints2D rhs)
	{
		return static_cast<BodyConstraints2D>(static_cast<int>(lhs) | static_cast<int>(rhs));
	}

	/*----------------------------------------------------------------------------
	| --- operator&: Returns the bits common to both BodyConstraints2D flags --- |
	----------------------------------------------------------------------------*/
	constexpr BodyConstraints2D operator&(BodyConstraints2D lhs, BodyConstraints2D rhs)
	{
		return static_cast<BodyConstraints2D>(static_cast<int>(lhs) & static_cast<int>(rhs));
	}

	/*-----------------------------------------------------------------------------------------------
	| --- operator^: Returns the bits present in exactly one of the two BodyConstraints2D flags --- |
	-----------------------------------------------------------------------------------------------*/
	constexpr BodyConstraints2D operator^(BodyConstraints2D lhs, BodyConstraints2D rhs)
	{
		return static_cast<BodyConstraints2D>(static_cast<int>(lhs) ^ static_cast<int>(rhs));
	}

	/*-------------------------------------------------------------------------------
	| --- operator~: Returns the bitwise complement of a BodyConstraints2D flag --- |
	-------------------------------------------------------------------------------*/
	constexpr BodyConstraints2D operator~(BodyConstraints2D value)
	{
		return static_cast<BodyConstraints2D>(~static_cast<int>(value));
	}

	/*---------------------------------------------------------------------
	| --- operator|=: Adds the given BodyConstraints2D flag(s) to lhs --- |
	---------------------------------------------------------------------*/
	constexpr BodyConstraints2D& operator|=(BodyConstraints2D& lhs, BodyConstraints2D rhs)
	{
		return lhs = lhs | rhs;
	}

	/*---------------------------------------------------------------------------------------------
	| --- operator&=: Keeps only the bits lhs shares with the given BodyConstraints2D flag(s) --- |
	---------------------------------------------------------------------------------------------*/
	constexpr BodyConstraints2D& operator&=(BodyConstraints2D& lhs, BodyConstraints2D rhs)
	{
		return lhs = lhs & rhs;
	}

	/*------------------------------------------------------------------------
	| --- operator^=: Toggles the given BodyConstraints2D flag(s) on lhs --- |
	------------------------------------------------------------------------*/
	constexpr BodyConstraints2D& operator^=(BodyConstraints2D& lhs, BodyConstraints2D rhs)
	{
		return lhs = lhs ^ rhs;
	}

	/*----------------------------------------------------------------------
	| --- HasFlag: Returns true if `value` has every bit in `flag` set --- |
	----------------------------------------------------------------------*/
	constexpr bool HasFlag(BodyConstraints2D value, BodyConstraints2D flag)
	{
		return (value & flag) == flag;
	}
}