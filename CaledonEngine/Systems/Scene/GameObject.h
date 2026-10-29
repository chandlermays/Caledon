/*------------------------------
| File: GameObject.h
| Author: Chandler Mays
------------------------------*/
#pragma once

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
		Scene* m_pScene;																					// Pointer to the Scene this GameObject is in
		GameObject* m_pParent;																				// Pointer to this GameObject's parent
		std::vector<std::unique_ptr<GameObject>> m_children;												// Vector of pointers to this GameObject's children

		std::string m_name;																					// The name of this GameObject
		std::string m_tag;																					// The tag of this GameObject
		int m_layer;																						// The physics layer of this GameObject
		bool m_isActive;																					// Whether this GameObject is active or not

		Transform* m_pTransform;																			// Pointer to this GameObject's Transform component
		std::vector<std::unique_ptr<Component>> m_components;												// Vector of pointers to Components attached to this GameObject

	public:
		GameObject();																						// Constructor
		~GameObject();																						// Destructor
		GameObject(const GameObject&) = delete;																// Prevent copy-construction
		GameObject& operator=(const GameObject&) = delete;													// Prevent copy-assignment
		GameObject(GameObject&&) = delete;																	// Prevent move-construction
		GameObject& operator=(GameObject&&) = delete;														// Prevent move-assignment

		bool Initialize();																					// Initializes this GameObject and its components
		void Update(float);																					// Updates this GameObject and its components

		Scene* GetScene() const;																			// Returns a pointer to the Scene this GameObject is in
		void SetScene(Scene* pScene);																		// Sets the Scene this GameObject is in

		GameObject* GetParent() const;																		// Returns a pointer to this GameObject's parent
		void SetParent(GameObject* pParent);																// Sets the parent of this GameObject

		const std::vector<std::unique_ptr<CaledonEngine::GameObject>>& GetChildren() const;					// Returns a vector of pointers to this GameObject's children
		void AddChild(std::unique_ptr<GameObject> pChild);													// Adds a child to this GameObject
		void RemoveChild(GameObject* pChild);																// Removes a child from this GameObject

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
		void RemoveComponent(Component* pComponent);														// Removes a component from this GameObject

		void Destroy();																						// Destroys this GameObject and its components

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