#include "Scene/BaseScene.h"
#include "GameObject/GameObject.h"


void BaseScene::Destroy()
{
	m_bLoaded = false;
	m_bInitialized = false;

	for (GameObject* rootObject : m_RootObjects)
	{
		if (rootObject != nullptr)
		{
			// Supprimer tous les composants de l'objet
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
	//CHECK(!name.empty());
	m_Name = name;
}

std::string BaseScene::GetName() const
{
	return m_Name;
}

bool BaseScene::IsUsingSaveFile() const
{
	//return m_bUsingSaveFile;
	return false;
}

std::vector<GameObject*>& BaseScene::GetRootObjects()
{
	return m_RootObjects;
}

//GameObject::id_t BaseScene::FirstObjectWithTag(const std::string& tag)
//{
//	for (GameObject* gameObject : m_RootObjects)
//	{
//		GameObjectID result = FindObjectWithTag(tag, gameObject);
//		if (result.IsValid())
//		{
//			return result;
//		}
//	}
//
//	return InvalidGameObjectID;
//}