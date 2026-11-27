/*------------------------------
| File: GameObject.h
| Author: Chandler Mays
------------------------------*/
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace CaledonEngine
{
	class Component;
	class Scene;
	class Transform;

	class GameObject
	{
	private:
		friend class Scene;																					// The Scene moves GameObjects in and out of the hierarchy during its end-of-frame flush

		std::uint64_t m_id;																					// The stable, unique identifier of this GameObject (never 0)
		Scene* m_pScene;																					// Pointer to the Scene this GameObject is in
		GameObject* m_pParent;																				// Pointer to this GameObject's parent
		std::vector<std::unique_ptr<GameObject>> m_children;												// Vector of pointers to this GameObject's children

		std::string m_name;																					// The name of this GameObject
		std::string m_tag;																					// The tag of this GameObject
		int m_layer;																						// The physics layer of this GameObject
		bool m_isActive;																					// Whether this GameObject is active or not
		bool m_isPendingDestroy;																			// Whether this GameObject is waiting to be destroyed at the end of the frame

		Transform* m_pTransform;																			// Pointer to this GameObject's Transform component
		std::vector<std::unique_ptr<Component>> m_components;												// Vector of pointers to Components attached to this GameObject

		static std::uint64_t GenerateID();																	// Returns a new random, non-zero 64-bit identifier
		void AttachChild(std::unique_ptr<GameObject> pChild);												// Takes ownership of a child (no initialization; used by AddChild, SetParent and the Scene)
		std::unique_ptr<GameObject> ReleaseChild(GameObject* pChild);										// Hands ownership of a child to the caller without destroying it
		std::unique_ptr<Component> ReleaseComponent(Component* pComponent);									// Hands ownership of a Component to the caller without destroying it
		void DestroyImmediate() noexcept;																	// Destroys the children and Components right away (destructor and end-of-frame flush only)

	public:
		GameObject();																						// Constructor
		~GameObject();																						// Destructor
		GameObject(const GameObject&) = delete;																// Prevent copy-construction
		GameObject& operator=(const GameObject&) = delete;													// Prevent copy-assignment
		GameObject(GameObject&&) = delete;																	// Prevent move-construction
		GameObject& operator=(GameObject&&) = delete;														// Prevent move-assignment

		bool Initialize();																					// Initializes this GameObject and its components
		void Update(float);																					// Updates this GameObject and its components

		std::uint64_t GetID() const noexcept;																// Returns the stable, unique identifier of this GameObject
		void SetID(std::uint64_t id) noexcept;																// Sets the identifier (used when loading a scene; 0 is ignored)

		Scene* GetScene() const;																			// Returns a pointer to the Scene this GameObject is in
		void SetScene(Scene* pScene);																		// Sets the Scene this GameObject (and its children) is in

		GameObject* GetParent() const;																		// Returns a pointer to this GameObject's parent
		bool SetParent(GameObject* pNewParent, bool worldPositionStays = true);								// Moves this GameObject under a new parent (nullptr = scene root), keeping its world pose by default
		bool IsAncestorOf(const GameObject* pGameObject) const;												// Returns whether this GameObject is a parent, grandparent, etc. of the given one

		const std::vector<std::unique_ptr<CaledonEngine::GameObject>>& GetChildren() const;					// Returns a vector of pointers to this GameObject's children
		GameObject* AddChild(std::unique_ptr<GameObject> pChild);											// Adds a new child (deferred to the end of the frame while its Scene is updating)
		void DetachChildren();																				// Moves every child to the scene root, keeping their world poses (Unity's DetachChildren)

		const std::string& GetName() const;																	// Returns the name of this GameObject
		void SetName(const std::string& name);																// Sets the name of this GameObject

		const std::string& GetTag() const;																	// Returns the tag of this GameObject
		void SetTag(const std::string& tag);																// Sets the tag of this GameObject

		int GetLayer() const noexcept;																		// Returns the physics layer of this GameObject
		void SetLayer(int layer) noexcept;																	// Sets the physics layer of this GameObject

		bool IsActive() const;																				// Returns whether this GameObject is active or not
		void SetActive(bool isActive);																		// Sets whether this GameObject is active or not

		const Transform& GetTransform() const;																// Returns the Transform of this GameObject
		Transform& GetTransform();																			// Returns the Transform of this GameObject

		template<typename T> T* GetComponent() const;														// Returns a reference to a component of type T attached to this GameObject
		template<typename T> T* GetComponentInChildren() const;												// Returns a reference to a component of type T attached to this GameObject, or any of its children
		template<typename T> T* GetComponentInParent() const;												// Returns a reference to a component of type T attached to this GameObject, or any of its parents
		template<typename T> std::vector<T*> GetComponents() const;											// Returns references to all components of type T attached to this GameObject
		template<typename T> std::vector<T*> GetComponentsInChildren() const;								// Returns references to all components of type T attached to this GameObject and its children
		template<typename T> std::vector<T*> GetComponentsInParent() const;									// Returns references to all components of type T attached to this GameObject and its parents

		const std::vector<std::unique_ptr<Component>>& GetAllComponents() const;							// Returns a vector of pointers to all Components attached to this GameObject
		void AddComponent(std::unique_ptr<Component> pComponent);											// Adds a component to this GameObject
		void RemoveComponent(Component* pComponent);														// Marks a component for removal at the end of the frame (the Transform can't be removed)

		void Destroy();																						// Marks this GameObject and its children for destruction at the end of the frame
		bool IsPendingDestroy() const noexcept;																// Returns whether this GameObject (or a parent) is waiting to be destroyed

		// void BroadcastMessage(method:...)																// Calls a method on every Component attached to this GameObject or any of its children
		// void SendMessage(method:...)																		// Calls a method on every Component attached to this GameObject
	};

	/*------------------------------------------------------------------------------------------------
	| --- GetComponent: Returns a reference to a component of type T attached to this GameObject --- |
	------------------------------------------------------------------------------------------------*/
	template<typename T>
	inline T* GameObject::GetComponent() const
	{
		for (const auto& pComponent : m_components)
		{
			if (T* pResult = dynamic_cast<T*>(pComponent.get()))
				return pResult;
		}
		return nullptr;
	}

	/*----------------------------------------------------------------------------------------------------------------------------------
	| --- GetComponentInChildren: Returns a reference to a component of type T attached to this GameObject, or any of its children --- |
	----------------------------------------------------------------------------------------------------------------------------------*/
	template<typename T>
	inline T* GameObject::GetComponentInChildren() const
	{
		if (T* pComponent = GetComponent<T>())
			return pComponent;

		for (const auto& pChild : m_children)
		{
			if (T* pComponent = pChild->GetComponentInChildren<T>())
				return pComponent;
		}
		return nullptr;
	}

	/*-------------------------------------------------------------------------------------------------------------------------------
	| --- GetComponentInParent: Returns a reference to a component of type T attached to this GameObject, or any of its parents --- |
	-------------------------------------------------------------------------------------------------------------------------------*/
	template<typename T>
	inline T* GameObject::GetComponentInParent() const
	{
		if (T* pComponent = GetComponent<T>())
			return pComponent;

		return m_pParent ? m_pParent->GetComponentInParent<T>() : nullptr;
	}

	/*---------------------------------------------------------------------------------------------------
	| --- GetComponents: Returns references to all components of type T attached to this GameObject --- |
	---------------------------------------------------------------------------------------------------*/
	template<typename T>
	inline std::vector<T*> GameObject::GetComponents() const
	{
		std::vector<T*> components;
		for (const auto& pComponent : m_components)
		{
			if (T* pResult = dynamic_cast<T*>(pComponent.get()))
			{
				components.emplace_back(pResult);
			}
		}
		return components;
	}

	/*------------------------------------------------------------------------------------------------------------------------------
	| --- GetComponentsInChildren: Returns references to all components of type T attached to this GameObject and its children --- |
	------------------------------------------------------------------------------------------------------------------------------*/
	template<typename T>
	inline std::vector<T*> GameObject::GetComponentsInChildren() const
	{
		std::vector<T*> components = GetComponents<T>();
		for (const auto& pChild : m_children)
		{
			std::vector<T*> childComponents = pChild->GetComponentsInChildren<T>();
			components.insert(components.end(), childComponents.begin(), childComponents.end());
		}
		return components;
	}

	/*---------------------------------------------------------------------------------------------------------------------------
	| --- GetComponentsInParent: Returns references to all components of type T attached to this GameObject and its parents --- |
	---------------------------------------------------------------------------------------------------------------------------*/
	template<typename T>
	inline std::vector<T*> GameObject::GetComponentsInParent() const
	{
		std::vector<T*> components = GetComponents<T>();
		if (m_pParent)
		{
			std::vector<T*> parentComponents = m_pParent->GetComponentsInParent<T>();
			components.insert(components.end(), parentComponents.begin(), parentComponents.end());
		}
		return components;
	}
}
