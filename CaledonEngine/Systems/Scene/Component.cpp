/*------------------------------
| File: Component.cpp
| Author: Chandler Mays
------------------------------*/
#include "Component.h"

/*---------------------------------------------------------------------------------------------------------------
| --- NOTE: If this class has nothing more than one-line defintions, consider making it a header-only class --- |
---------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------
| --- Public Method Definitions --- |
-----------------------------------*/
/*-------------------------------------------------------------------
| --- Constructor: Constructs the Component with default values --- |
-------------------------------------------------------------------*/
CaledonEngine::Component::Component()
	: m_pOwner{ nullptr }
	, m_isActive{ true }
{ }

/*-----------------------------------------------------------
| --- GetOwner: Gets a pointer to the owning GameObject --- |
-----------------------------------------------------------*/
CaledonEngine::GameObject* CaledonEngine::Component::GetOwner() const
{
	return m_pOwner;
}

/*----------------------------------------------
| --- SetOwner: Sets the owning GameObject --- |
----------------------------------------------*/
void CaledonEngine::Component::SetOwner(GameObject* owner)
{
	m_pOwner = owner;
}

/*------------------------------------------------------------
| --- GetID: Gets the unique identifier of the Component --- |
------------------------------------------------------------*/
const std::string& CaledonEngine::Component::GetID() const
{
	return m_id;
}

/*------------------------------------------------------------
| --- SetID: Sets the unique identifier of the Component --- |
------------------------------------------------------------*/
void CaledonEngine::Component::SetID(const std::string& id)
{
	m_id = id;
}

/*---------------------------------------------------------------
| --- IsActive: Gets whether the Component is active or not --- |
---------------------------------------------------------------*/
bool CaledonEngine::Component::IsActive() const
{
	return m_isActive;
}

/*----------------------------------------------------------------
| --- SetActive: Sets whether the Component is active or not --- |
----------------------------------------------------------------*/
void CaledonEngine::Component::SetActive(bool isActive)
{
	m_isActive = isActive;
}