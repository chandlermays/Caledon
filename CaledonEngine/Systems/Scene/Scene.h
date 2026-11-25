/*------------------------------
| File: Scene.h
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
	class GameObject;
	class PhysicsScene2D;

	class Scene
	{
	private:
		std::string m_name;																							// The name of this Scene
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;														// The GameObjects in this Scene
		bool m_isActive;																							// Whether this Scene is active or not
		bool m_isDirty;																								// Whether this Scene has unsaved changes (needs to be saved)
		bool m_isLoaded;																							// Whether this Scene has completed loading and objects have been initialized/enabled
		std::string m_filePath;																						// The file path this Scene was loaded from
		PhysicsScene2D* m_pPhysicsScene2D;																			// The PhyicsScene2D this Scene's physics components registers with

		struct PendingAdd
		{
			std::unique_ptr<GameObject> pGameObject;																// The new GameObject waiting to join the hierarchy
			GameObject* pParent;																					// Where it goes (nullptr = scene root)
		};

		std::vector<PendingAdd> m_pendingAdds;																		// GameObjects created during Update, added at the end of the frame
		std::vector<GameObject*> m_pendingDestroyObjects;															// GameObjects marked with Destroy(), destroyed at the end of the frame
		std::vector<Component*> m_pendingDestroyComponents;															// Components marked with RemoveComponent(), destroyed at the end of the frame
		bool m_isUpdating;																							// Whether Update is walking the GameObjects (adding must wait until the flush)

		void AttachGameObject(std::unique_ptr<GameObject> pGameObject);												// Takes ownership of a root GameObject (no initialization)
		std::unique_ptr<GameObject> ReleaseGameObject(GameObject* pGameObject);										// Hands ownership of a root GameObject to the caller without destroying it
		bool RetargetPendingAdd(GameObject* pGameObject, GameObject* pNewParent);									// Changes where a queued GameObject will be added; false if it isn't queued
		void QueueDestroy(GameObject* pGameObject);																	// Queues a GameObject marked with Destroy()
		void QueueDestroy(Component* pComponent);																	// Queues a Component marked with RemoveComponent()
		void QueueMarkedInSubtree(GameObject* pGameObject);															// Queues GameObjects that were marked with Destroy() before they joined this Scene

		friend class GameObject;																					// GameObject::SetParent, Destroy and RemoveComponent go through the private methods above

	public:
		Scene();																									// Constructor
		~Scene();																									// Destructor
		Scene(const Scene&) = delete;																				// Prevent copy-construction
		Scene& operator=(const Scene&) = delete;																	// Prevent copy-assignment
		Scene(Scene&&) = delete;																					// Prevent move-construction
		Scene& operator=(Scene&&) = delete;																			// Prevent move-assignment

		bool Initialize();																							// Initializes this Scene and its GameObjects
		void Update(float);																							// Updates this Scene and its GameObjects

		const std::string& GetName() const;																			// Returns the name of this Scene
		void SetName(const std::string& name);																		// Sets the name of this Scene

		bool IsActive() const;																						// Returns whether this Scene is active or not
		void SetActive(bool isActive);																				// Sets whether this Scene is active or not

		bool IsDirty() const;																						//
		void SetDirty(bool dirty);																					//
		void MarkClean();																							// 
		bool IsValid() const;																						// Checks if this Scene is valid

		const std::string& GetFilePath() const;																		//
		void SetFilePath(const std::string& filePath);																// 

		PhysicsScene2D* GetPhysicsScene2D() const noexcept;															//
		void SetPhysicsScene2D(PhysicsScene2D* pPhysicsScene);														//

		GameObject* AddGameObject(std::unique_ptr<GameObject> pGameObject, GameObject* pParent = nullptr);			// Adds a GameObject (queued while updating; initialized if the Scene is loaded)
		void FlushPendingChanges();																					// Adds the queued GameObjects, then destroys the marked Components and GameObjects
		bool HasPendingChanges() const noexcept;																	// Returns whether anything is waiting for the end-of-frame flush

		bool IsLoaded() const noexcept;																				// Returns whether the Scene has been initialized
		bool Contains(const GameObject* pGameObject) const;															// Returns whether the GameObject is in this Scene's hierarchy (not still queued)
		GameObject* FindGameObject(std::uint64_t id) const;															// Finds a GameObject (roots, children and queued ones) by its ID
		const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const;										// Returns a vector of unique pointers to the GameObjects in this Scene
		void Clear();																								// Clears all GameObjects from this Scene
	};
}
