#include "Scene/BaseScene.h"
#include "GameObject/GameObject.h"
#include <fstream>
#include <string>
#include <iostream>


void BaseScene::Destroy()
{
	m_bLoaded = false;
	m_bInitialized = false;

	for (GameObject* rootObject : m_RootObjects)
	{
		if (rootObject != nullptr)
		{
			for (Component* component : rootObject->GetComponents())
			{
				rootObject->RemoveComponent(component);
			}
			delete rootObject;
		}
	}
	m_RootObjects.clear();
}


GameObject* BaseScene::AddRootObject(GameObject* gameObject)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	m_PendingAddObjects.push_back(gameObject);

	return gameObject;
}

void BaseScene::RemoveAllObjects()
{
	for (GameObject* rootObject : m_RootObjects)
	{
		m_PendingDestroyObjects.push_back(rootObject->GetId());
	}
}

bool BaseScene::IsInitialized() const
{
	return m_bInitialized;
}

void BaseScene::SetName(const std::string& name)
{
	m_Name = name;
}

std::string BaseScene::GetName() const
{
	return m_Name;
}

bool BaseScene::IsUsingSaveFile() const
{
	return false;
}

std::vector<GameObject*>& BaseScene::GetRootObjects()
{
	return m_RootObjects;
}


std::string BaseScene::GetFileName() const
{
	return m_FileName;
}

bool BaseScene::SetFileName(const std::string& fileName, bool bDeletePreviousFiles)
{
	bool success = false;

	if (fileName == m_FileName)
	{
		return true;
	}
}

std::string BaseScene::GetDefaultRelativeFilePath() const
{
	//A MODIFIIIIEEEEEEEEEERRRRRR
	const std::string DEFAULT_SCENE_DIRECTORY = "assets/scenes/";
	return DEFAULT_SCENE_DIRECTORY + m_FileName;
}


bool FileExists(const std::string& filePath) {
	std::ifstream file(filePath);
	return file.good();
}


void BaseScene::DeleteSaveFiles()
{
	const std::string defaultSaveFilePath = "assets/scenes/" + m_FileName;
	const std::string savedSaveFilePath = "saved/scenes/" + m_FileName;

	bool bDefaultFileExists = FileExists(defaultSaveFilePath);
	bool bSavedFileExists = FileExists(savedSaveFilePath);

	if (bSavedFileExists || bDefaultFileExists)
	{
		std::cout << "Deleting scene's save files from " << m_FileName << std::endl;

		if (bDefaultFileExists)
		{
			if (remove(defaultSaveFilePath.c_str()) != 0)
			{
				std::cerr << "Error deleting default save file: " << defaultSaveFilePath << std::endl;
			}
			else
			{
				std::cout << "Default save file deleted successfully." << std::endl;
			}
		}

		if (bSavedFileExists)
		{
			if (remove(savedSaveFilePath.c_str()) != 0)
			{
				std::cerr << "Error deleting saved save file: " << savedSaveFilePath << std::endl;
			}
			else
			{
				std::cout << "Saved save file deleted successfully." << std::endl;
			}
		}
	}
}

bool Contains(const std::vector<GameObject::id_t>& container, const GameObject::id_t& value)
{
	return std::find(container.begin(), container.end(), value) != container.end();
}


void BaseScene::RemoveObject(const GameObject::id_t& gameObjectID, bool bDestroy)
{
	if (bDestroy)
	{
		if (!Contains(m_PendingDestroyObjects, gameObjectID))
		{
			m_PendingDestroyObjects.push_back(gameObjectID);
		}
	}
	else
	{
		if (!Contains(m_PendingRemoveObjects, gameObjectID))
		{
			m_PendingRemoveObjects.push_back(gameObjectID);
		}
	}
}

void BaseScene::RemoveObject(GameObject* gameObject, bool bDestroy)
{
	RemoveObject(gameObject->GetId(), bDestroy);
}



void BaseScene::RemoveObjects(const std::vector<GameObject::id_t>& gameObjects, bool bDestroy)
{
	if (bDestroy)
	{
		m_PendingDestroyObjects.insert(m_PendingDestroyObjects.end(), gameObjects.begin(), gameObjects.end());
	}
	else
	{
		m_PendingRemoveObjects.insert(m_PendingRemoveObjects.end(), gameObjects.begin(), gameObjects.end());
	}
}

void BaseScene::RemoveObjects(const std::vector<GameObject*>& gameObjects, bool bDestroy)
{
	if (bDestroy)
	{
		m_PendingDestroyObjects.reserve(m_PendingDestroyObjects.size() + gameObjects.size());
		for (GameObject* gameObject : gameObjects)
		{
			if (!Contains(m_PendingDestroyObjects, gameObject->GetId()))
			{
				m_PendingDestroyObjects.push_back(gameObject->GetId());
			}
		}
	}
	else
	{
		m_PendingRemoveObjects.reserve(m_PendingRemoveObjects.size() + gameObjects.size());
		for (GameObject* gameObject : gameObjects)
		{
			if (!Contains(m_PendingRemoveObjects, gameObject->GetId()))
			{
				m_PendingRemoveObjects.push_back(gameObject->GetId());
			}
		}
	}
}



GameObject* BaseScene::CreateGameObject() {
	GameObject* gameObject = new GameObject();
	m_PendingAddObjects.push_back(gameObject);
	return gameObject;
}

void BaseScene::DestroyGameObject(GameObject* gameObject) {
	if (gameObject != nullptr) {
		RemoveObject(gameObject, true);
	}
}

GameObject* BaseScene::GetGameObjectById(const GameObject::id_t& gameObjectID) {
	for (GameObject* rootObject : m_RootObjects) {
		if (rootObject->GetId() == gameObjectID) {
			return rootObject;
		}
	}
	return nullptr;
}

std::vector<GameObject*> BaseScene::FindGameObjectsByName(const std::string& name) {
	std::vector<GameObject*> result;
	for (GameObject* rootObject : m_RootObjects) {
		std::vector<GameObject*> found = rootObject->FindChildrenByName(name);
		result.insert(result.end(), found.begin(), found.end());
	}
	return result;
}



void BaseScene::Render(lve::LveWindow* _window)
{
	// Rendu de chaque objet de la scène
	for (GameObject* rootObject : m_RootObjects)
	{
		//RenderObject(rootObject);
	}
}

void BaseScene::Update(float deltaTime)
{
	// Mettez à jour chaque objet de la scène avec le delta time
	for (GameObject* rootObject : m_RootObjects)
	{
		rootObject->Update(deltaTime); // Mettez à jour chaque objet avec le delta time
	}
}