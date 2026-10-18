/*------------------------------
| File: Collision2D.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include "ContactPoint2D.h"
#include "Utilities/Math/Vector2.h"

#include <span>
#include <vector>

// Contains collision details for 2D physics callbacks, including the involved colliders,
// rigid bodies, and contact points.
// Exposed scene objects are non-owning pointers valid only for the duration of the callback.
// Do not retain Collision2D references across frames.

namespace CaledonEngine
{
	class Collider2D;
	class GameObject;
	class RigidBody2D;
	class Transform;

	class Collision2D
	{
	private:
		Collider2D* m_pCollider;																// 
		Collider2D* m_pOtherCollider;															// 
		RigidBody2D* m_pRigidBody;																// 
		RigidBody2D* m_pOtherRigidBody;															// 
		Vector2f m_relativeVelocity;															// 
		std::vector<ContactPoint2D> m_contacts;													// 
		bool m_enabled;																			// 

	public:
		Collision2D(Collider2D* pCollider, Collider2D* pOtherCollider,							// Constructor
			RigidBody2D* pRigidBody, RigidBody2D* pOtherRigidBody,
			const Vector2f& relativeVelocity)
			: m_pCollider{ pCollider }
			, m_pOtherCollider{ pOtherCollider }
			, m_pRigidBody{ pRigidBody }
			, m_pOtherRigidBody{ pOtherRigidBody }
			, m_relativeVelocity{ relativeVelocity }
			, m_enabled{ true }
		{ }

		Collider2D* GetCollider() const noexcept						{ return m_pCollider; }								//
		Collider2D* GetOtherCollider() const noexcept					{ return m_pOtherCollider; }						//
		RigidBody2D* GetRigidBody() const noexcept						{ return m_pRigidBody; }							//
		RigidBody2D* GetOtherRigidBody() const noexcept					{ return m_pOtherRigidBody; }						//
		const Vector2f& GetRelativeVelocity() const noexcept			{ return m_relativeVelocity; }						//
		bool IsEnabled() const noexcept									{ return m_enabled; }								//
		void SetEnabled(bool enabled) noexcept							{ m_enabled = enabled; }							//

		GameObject* GetGameObject() const noexcept;																			//
		Transform* GetTransform() const noexcept;																			//

		int GetContactCount() const noexcept							{ return static_cast<int>(m_contacts.size()); }		//
		void AddContact(const ContactPoint2D& contact)					{ m_contacts.push_back(contact); }					//

		std::span<const ContactPoint2D> GetContacts() const noexcept	{ return m_contacts; }								//

		/*---------------------------------------------------------------------------------------
		| --- GetContact:
		---------------------------------------------------------------------------------------*/
		const ContactPoint2D* GetContact(int index) const noexcept
		{
			if (index < 0 || index >= GetContactCount())
				return nullptr;

			return &m_contacts[static_cast<std::size_t>(index)];
		}

		/*---------------------------------------------------------------------------------------
		| --- GetContacts:
		---------------------------------------------------------------------------------------*/
		int GetContacts(std::span<ContactPoint2D> contacts) const noexcept
		{
			const std::size_t count = contacts.size() < m_contacts.size() ? contacts.size() : m_contacts.size();
			for (std::size_t i = 0; i < count; ++i)
				contacts[i] = m_contacts[i];

			return static_cast<int>(count);
		}
	};
}