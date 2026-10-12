/*------------------------------
| File: Component.inl
| Author: Chandler Mays
------------------------------*/
#pragma once
#include "Systems/Scene/GameObject.h"

namespace CaledonEngine
{
	/*----------------------------------------------------------------------------------------------------
	| --- GetComponent: Returns a reference to a component of type T attached to the same GameObject --- |
	----------------------------------------------------------------------------------------------------*/
	template <typename T>
	inline T* Component::GetComponent() const
	{
		if (m_pOwner == nullptr)
			return nullptr;

		return m_pOwner->GetComponent<T>();
	}

	/*----------------------------------------------------------------------------------------------------------------------------------
	| --- GetComponentInChildren: Returns a reference to a component of type T attached to same GameObject, or any of its children --- |
	----------------------------------------------------------------------------------------------------------------------------------*/
	template <typename T>
	inline T* Component::GetComponentInChildren() const
	{
		if (m_pOwner == nullptr)
			return nullptr;
		return m_pOwner->GetComponentInChildren<T>();
	}

	/*-------------------------------------------------------------------------------------------------------------------------------
	| --- GetComponentInParent: Returns a reference to a component of type T attached to same GameObject, or any of its parents --- |
	-------------------------------------------------------------------------------------------------------------------------------*/
	template <typename T>
	inline T* Component::GetComponentInParent() const
	{
		if (m_pOwner == nullptr)
			return nullptr;
		return m_pOwner->GetComponentInParent<T>();
	}

	/*-------------------------------------------------------------------------------------------------------
	| --- GetComponents: Returns references to all components of type T attached to the same GameObject --- |
	-------------------------------------------------------------------------------------------------------*/
	template <typename T>
	inline std::vector<T*> Component::GetComponents() const
	{
		if (m_pOwner == nullptr)
			return {};
		return m_pOwner->GetComponents<T>();
	}

	/*------------------------------------------------------------------------------------------------------------------------------
	| --- GetComponentsInChildren: Returns references to all components of type T attached to same GameObject and its children --- |
	------------------------------------------------------------------------------------------------------------------------------*/
	template <typename T>
	inline std::vector<T*> Component::GetComponentsInChildren() const
	{
		if (m_pOwner == nullptr)
			return {};
		return m_pOwner->GetComponentsInChildren<T>();
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	| --- GetComponentsInParent: Returns references to all components of type T attached to same GameObject and its parents --- |
	---------------------------------------------------------------------------------------------------------------------------*/
	template <typename T>
	inline std::vector<T*> Component::GetComponentsInParent() const
	{
		if (m_pOwner == nullptr)
			return {};
		return m_pOwner->GetComponentsInParent<T>();
	}
}