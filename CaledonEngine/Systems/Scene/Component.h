/*------------------------------
| File: Component.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <string>
#include <vector>

namespace CaledonEngine
{
	class EngineContext;
	class GameObject;

	class Component
	{
	protected:
		GameObject* m_pOwner;																// Pointer to the owning GameObject
		std::string m_id;																	// The unique identifier of the Component
		bool m_isActive;																	// Whether the Component is active or not

	public:
		Component();																		// Constructor
		virtual ~Component() = default;														// Destructor
		Component(const Component&) = delete;												// Prevent copy-construction
		Component& operator=(const Component&) = delete;									// Prevent copy-assignment
		Component(Component&&) = delete;													// Prevent move-construction
		Component& operator=(Component&&) = delete;											// Prevent move-assignment

		virtual bool Initialize(EngineContext& context) { return true; }					// Initializes the Component
		virtual void Update(float) {};														// Updates the Component, if applicable
		virtual void Render() {};															// Renders the Component, if applicable

		virtual const std::string& GetTypeName() const = 0;									// Returns the type name of the Component

		GameObject* GetOwner() const;														// Returns a pointer to the owning GameObject
		void SetOwner(GameObject* owner);													// Sets the owning GameObject

		const std::string& GetID() const;													// Returns the unique identifier of the Component
		void SetID(const std::string& id);													// Sets the unique identifier of the Component

		bool IsActive() const;																// Returns whether the Component is active or not
		void SetActive(bool isActive);														// Sets whether the Component is active or not

		template<typename T> T* GetComponent() const;										// Returns a reference to a component of type T attached to the same GameObject
		template<typename T> T* GetComponentInChildren() const;								// Returns a reference to a component of type T attached to same GameObject, or any of its children
		template<typename T> T* GetComponentInParent() const;								// Returns a reference to a component of type T attached to same GameObject, or any of its parents
		template<typename T> std::vector<T*> GetComponents() const;							// Returns references to all components of type T attached to the same GameObject
		template<typename T> std::vector<T*> GetComponentsInChildren() const;				// Returns references to all components of type T attached to same GameObject, or any of its children
		template<typename T> std::vector<T*> GetComponentsInParent() const;					// Returns references to all components of type T attached to same GameObject, or any of its parents
	};
}

#include "Component.inl"