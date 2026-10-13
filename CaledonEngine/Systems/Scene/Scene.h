/*------------------------------
| File: Scene.h
| Author: Chandler Mays
------------------------------*/
#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CaledonEngine
{
	class GameObject;

	class Scene
	{
	private:
		std::string m_name;															// The name of this Scene
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;						// The GameObjects in this Scene
		bool m_isActive;															// Whether this Scene is active or not
		bool m_isDirty;																// Whether this Scene has unsaved changes (needs to be saved)
		bool m_isLoaded;															// Whether this Scene has completed loading and objects have been initialized/enabled
		std::string m_filePath;														// The file path this Scene was loaded from

	public:
		Scene();																	// Constructor
		~Scene();																	// Destructor
		Scene(const Scene&) = delete;												// Prevent copy-construction
		Scene& operator=(const Scene&) = delete;									// Prevent copy-assignment
		Scene(Scene&&) = delete;													// Prevent move-construction
		Scene& operator=(Scene&&) = delete;											// Prevent move-assignment

		bool Initialize();															// Initializes this Scene and its GameObjects
		void Update(float);															// Updates this Scene and its GameObjects

		const std::string& GetName() const;											// Returns the name of this Scene
		void SetName(const std::string& name);										// Sets the name of this Scene

		bool IsActive() const;														// Returns whether this Scene is active or not
		void SetActive(bool isActive);												// Sets whether this Scene is active or not

		bool IsDirty() const;														//
		void SetDirty(bool dirty);													//
		void MarkClean();															// 
		bool IsValid() const;														// Checks if this Scene is valid

		const std::string& GetFilePath() const;										//
		void SetFilePath(const std::string& filePath);								// 

		void AddGameObject(std::unique_ptr<GameObject> pGameObject);				// Adds a GameObject to this Scene
		void RemoveGameObject(GameObject* pGameObject);								// Removes a GameObject from this Scene
		const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const;		// Returns a vector of unique pointers to the GameObjects in this Scene
		void Clear();																// Clears all GameObjects from this Scene
	};
}