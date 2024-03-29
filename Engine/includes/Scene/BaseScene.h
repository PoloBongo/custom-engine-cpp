#pragma once

// std
#include <string>

// engine
#include "GameObject/GameObject.h"

class BaseScene final
{
public:
	explicit BaseScene(const std::string& fileName);
	~BaseScene() = default;

	BaseScene(const BaseScene&) = delete;
	BaseScene& operator=(const BaseScene&) = delete;

	void Initialize();
	void PostInitialize();
	void Destroy();
	void FixedUpdate();
	void Update();
	void LateUpdate();
	void Render();

	bool IsInitialized() const;

	void SetName(const std::string& name);
	std::string GetName() const;
	std::string GetDefaultRelativeFilePath() const;
	std::string GetRelativeFilePath() const;
	std::string GetShortRelativeFilePath() const;
	// Creates a new file at the specified location and copies this scene's data in to it
	// Always copies saved file if exists, old files can optionally be deleted
	bool SetFileName(const std::string& fileName, bool bDeletePreviousFiles);
	//std::string GetFileName() const;

	bool IsUsingSaveFile() const;

	void DeleteSaveFiles();

	std::vector<GameObject*>& GetRootObjects();

	GameObject* AddRootObject(GameObject* gameObject);
	GameObject* AddRootObjectImmediate(GameObject* gameObject);
	GameObject* AddChildObject(GameObject* parent, GameObject* child);
	GameObject* AddChildObjectImmediate(GameObject* parent, GameObject* child);
	GameObject* AddSiblingObjectImmediate(GameObject* gameObject, GameObject* newSibling);

	void SetRootObjectIndex(GameObject* rootObject, uint32_t newIndex);


	void RemoveAllObjects(); // Removes and destroys all objects in scene at end of frame
	void RemoveAllObjectsImmediate();  // Removes and destroys all objects in scene
	void RemoveAllEditorObjectsImmediate();
	void RemoveObject(const GameObject::id_t& gameObjectID, bool bDestroy);
	void RemoveObject(GameObject* gameObject, bool bDestroy);
	void RemoveObjectImmediate(const GameObject::id_t& gameObjectID, bool bDestroy);
	void RemoveObjectImmediate(GameObject* gameObject, bool bDestroy);
	void RemoveObjects(const std::vector<GameObject::id_t>& gameObjects, bool bDestroy);
	void RemoveObjects(const std::vector<GameObject*>& gameObjects, bool bDestroy);
	void RemoveObjectsImmediate(const std::vector<GameObject::id_t>& gameObjects, bool bDestroy);
	void RemoveObjectsImmediate(const std::vector<GameObject*>& gameObjects, bool bDestroy);

	GameObject::id_t FirstObjectWithTag(const std::string& tag);

	std::string m_Name;
	std::string m_FileName;

	std::vector<GameObject::id_t> m_PendingDestroyObjects; // Objects to destroy at LateUpdate this frame
	std::vector<GameObject*> m_PendingAddObjects; // Objects to add as root objects at LateUpdate
	std::vector<GameObject*> m_RootObjects;


	bool m_bInitialized = false;
	bool m_bLoaded = false;
};

