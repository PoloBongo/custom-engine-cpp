#include "Scene/BaseScene.h"
#include "GameObject/GameObject.h"
#include <fstream>
#include <string>
#include <iostream>

/**
 * @brief Destructeur de la scène.
 * Réinitialise les indicateurs de chargement et d'initialisation, et détruit tous les objets racine de la scène.
 */
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

/**
 * @brief Ajoute un objet de jeu en tant qu'objet racine à la scène.
 * @param gameObject Pointeur vers l'objet de jeu à ajouter.
 * @return Pointeur vers l'objet de jeu ajouté.
 */
GameObject* BaseScene::AddRootObject(GameObject* gameObject)
{
	if (gameObject == nullptr)
	{
		return nullptr;
	}

	m_PendingAddObjects.push_back(gameObject);

	return gameObject;
}

/**
 * @brief Supprime tous les objets de la scène à la fin du frame.
 */
void BaseScene::RemoveAllObjects()
{
	for (GameObject* rootObject : m_RootObjects)
	{
		m_PendingDestroyObjects.push_back(rootObject->GetId());
	}
}

/**
 * @brief Vérifie si la scène est initialisée.
 * @return true si la scène est initialisée, sinon false.
 */
bool BaseScene::IsInitialized() const
{
	return m_bInitialized;
}

/**
 * @brief Définit le nom de la scène.
 * @param name Nouveau nom de la scène.
 */
void BaseScene::SetName(const std::string& name)
{
	m_Name = name;
}

/**
 * @brief Récupère le nom de la scène.
 * @return Le nom de la scène.
 */
std::string BaseScene::GetName() const
{
	return m_Name;
}

/**
 * @brief Vérifie si la scène utilise un fichier de sauvegarde.
 * @return true si la scène utilise un fichier de sauvegarde, sinon false.
 */
bool BaseScene::IsUsingSaveFile() const
{
	return false;
}

/**
 * @brief Récupère les objets racine de la scène.
 * @return Une référence vers le vecteur contenant les objets racine de la scène.
 */
std::vector<GameObject*>& BaseScene::GetRootObjects()
{
	return m_RootObjects;
}


/**
 * @brief Récupère le nom du fichier de la scène.
 * @return Le nom du fichier de la scène.
 */
std::string BaseScene::GetFileName() const
{
	return m_FileName;
}

/**
 * @brief Définit le nom du fichier de la scène.
 * @param fileName Nouveau nom du fichier de la scène.
 * @param bDeletePreviousFiles Indique s'il faut supprimer les fichiers précédents.
 * @return true si le nom du fichier est défini avec succès, sinon false.
 */
bool BaseScene::SetFileName(const std::string& fileName, bool bDeletePreviousFiles)
{
	bool success = false;

	if (fileName == m_FileName)
	{
		return true;
	}
}

/**
 * @brief Récupère le chemin de fichier par défaut de la scène.
 * @return Le chemin de fichier par défaut de la scène.
 */
std::string BaseScene::GetDefaultRelativeFilePath() const
{
	//A MODIFIIIIEEEEEEEEEERRRRRR
	const std::string DEFAULT_SCENE_DIRECTORY = "assets/scenes/";
	return DEFAULT_SCENE_DIRECTORY + m_FileName;
}

/**
 * @brief Vérifie si un fichier existe.
 * @param filePath Chemin du fichier à vérifier.
 * @return true si le fichier existe, sinon false.
 */

bool FileExists(const std::string& filePath) {
	std::ifstream file(filePath);
	return file.good();
}

/**
 * @brief Supprime les fichiers de sauvegarde de la scène.
 */
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

/**
 * @brief Vérifie si un élément est contenu dans un vecteur.
 * @param container Vecteur dans lequel chercher.
 * @param value Valeur à chercher.
 * @return true si la valeur est trouvée dans le vecteur, sinon false.
 */
bool Contains(const std::vector<GameObject::id_t>& container, const GameObject::id_t& value)
{
	return std::find(container.begin(), container.end(), value) != container.end();
}

/**
 * @brief Supprime un objet de jeu de la scène.
 * @param gameObjectID Identifiant de l'objet de jeu à supprimer.
 * @param bDestroy Indique s'il faut détruire l'objet de jeu.
 */
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

/**
 * @brief Supprime un objet de jeu de la scène.
 * @param gameObject Pointeur vers l'objet de jeu à supprimer.
 * @param bDestroy Indique s'il faut détruire l'objet de jeu.
 */
void BaseScene::RemoveObject(GameObject* gameObject, bool bDestroy)
{
	RemoveObject(gameObject->GetId(), bDestroy);
}


/**
 * @brief Supprime plusieurs objets de jeu de la scène.
 * @param gameObjects Vecteur contenant les identifiants des objets de jeu à supprimer.
 * @param bDestroy Indique s'il faut détruire les objets de jeu.
 */
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

/**
 * @brief Supprime plusieurs objets de jeu de la scène.
 * @param gameObjects Vecteur contenant les pointeurs vers les objets de jeu à supprimer.
 * @param bDestroy Indique s'il faut détruire les objets de jeu.
 */
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


/**
 * @brief Crée un nouvel objet de jeu.
 * @return Pointeur vers le nouvel objet de jeu créé.
 */
GameObject* BaseScene::CreateGameObject() {
	GameObject* gameObject = new GameObject();
	m_PendingAddObjects.push_back(gameObject);
	return gameObject;
}

/**
 * @brief Détruit un objet de jeu.
 * @param gameObject Pointeur vers l'objet de jeu à détruire.
 */

void BaseScene::DestroyGameObject(GameObject* gameObject) {
	if (gameObject != nullptr) {
		RemoveObject(gameObject, true);
	}
}

/**
 * @brief Récupère un objet de jeu par son identifiant.
 * @param gameObjectID Identifiant de l'objet de jeu à récupérer.
 * @return Pointeur vers l'objet de jeu correspondant à l'identifiant.
 */
GameObject* BaseScene::GetGameObjectById(const GameObject::id_t& gameObjectID) {
	for (GameObject* rootObject : m_RootObjects) {
		if (rootObject->GetId() == gameObjectID) {
			return rootObject;
		}
	}
	return nullptr;
}

/**
 * @brief Recherche des objets de jeu par leur nom.
 * @param name Nom des objets de jeu à rechercher.
 * @return Vecteur contenant les pointeurs vers les objets de jeu trouvés.
 */
std::vector<GameObject*> BaseScene::FindGameObjectsByName(const std::string& name) {
	std::vector<GameObject*> result;
	for (GameObject* rootObject : m_RootObjects) {
		std::vector<GameObject*> found = rootObject->FindChildrenByName(name);
		result.insert(result.end(), found.begin(), found.end());
	}
	return result;
}


/**
 * @brief Effectue le rendu de la scène.
 * @param _window Fenêtre de rendu.
 */
void BaseScene::Render(lve::LveWindow* _window)
{
	// Rendu de chaque objet de la scène
	for (GameObject* rootObject : m_RootObjects)
	{
		//RenderObject(rootObject);
	}
}

/**
 * @brief Met à jour la scène.
 * @param deltaTime Temps écoulé depuis la dernière mise à jour.
 */
void BaseScene::Update(float deltaTime)
{
	// Mettez à jour chaque objet de la scène avec le delta time
	for (GameObject* rootObject : m_RootObjects)
	{
		rootObject->Update(deltaTime); // Mettez à jour chaque objet avec le delta time
	}
}