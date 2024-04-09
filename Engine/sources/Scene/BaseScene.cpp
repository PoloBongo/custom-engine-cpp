#include "Scene/BaseScene.h"
#include <fstream>
#include <iostream>
#include <string>

#include "CoreEngine.h"
#include "rhi.h"
#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/PreGameObject/PlaneGameObject.h"


/**
 * @brief Ajoute un objet de jeu en tant qu'objet racine à la scène.
 * @param _gameObject Pointeur vers l'objet de jeu à ajouter.
 * @return Pointeur vers l'objet de jeu ajouté.
 */
GameObject* BaseScene::AddRootObject(GameObject* _gameObject)
{
	if (_gameObject == nullptr) return nullptr;

	pendingAddObjects.push_back(_gameObject);

	return _gameObject;
}

/**
 * @brief Supprime tous les objets de la scène à la fin du frame.
 */
void BaseScene::RemoveAllObjects()
{
	for (const GameObject* root_object : rootObjects)
	{
		pendingDestroyObjects.push_back(root_object->GetId());
	}
}

/**
 * @brief Vérifie si la scène est initialisée.
 * @return true si la scène est initialisée, sinon false.
 */
bool BaseScene::IsInitialized() const
{
	return bInitialized;
}

/**
 * @brief Définit le nom de la scène.
 * @param _name Nouveau nom de la scène.
 */
void BaseScene::SetName(const std::string& _name)
{
	name = _name;
}

/**
 * @brief Récupère le nom de la scène.
 * @return Le nom de la scène.
 */
std::string BaseScene::GetName() const
{
	return name;
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
	return rootObjects;
}


/**
 * @brief Récupère le nom du fichier de la scène.
 * @return Le nom du fichier de la scène.
 */
std::string BaseScene::GetFileName() const
{
	return fileName;
}

/**
 * @brief Définit le nom du fichier de la scène.
 * @param _fileName Nouveau nom du fichier de la scène.
 * @param _bDeletePreviousFiles Indique s'il faut supprimer les fichiers précédents.
 * @return true si le nom du fichier est défini avec succès, sinon false.
 */
bool BaseScene::SetFileName(const std::string& _fileName, bool _bDeletePreviousFiles) const
{
	bool success = false;

	if (fileName == _fileName) success = true;
	return success;
}

/**
 * @brief Récupère le chemin de fichier par défaut de la scène.
 * @return Le chemin de fichier par défaut de la scène.
 */
std::string BaseScene::GetDefaultRelativeFilePath() const
{
	//A MODIFIIIIEEEEEEEEEERRRRRR
	const std::string DEFAULT_SCENE_DIRECTORY = "assets/scenes/";
	return DEFAULT_SCENE_DIRECTORY + fileName;
}

/**
 * @brief Vérifie si un fichier existe.
 * @param _filePath Chemin du fichier à vérifier.
 * @return true si le fichier existe, sinon false.
 */

bool BaseScene::FileExists(const std::string& _filePath)
{
	const std::ifstream file(_filePath);
	return file.good();
}

/**
 * @brief Supprime les fichiers de sauvegarde de la scène.
 */
void BaseScene::DeleteSaveFiles()
{
	const std::string default_save_file_path = "assets/scenes/" + fileName;
	const std::string saved_save_file_path   = "saved/scenes/" + fileName;

	const bool b_default_file_exists = FileExists(default_save_file_path);

	if (const bool b_saved_file_exists = FileExists(saved_save_file_path); b_saved_file_exists || b_default_file_exists)
	{
		std::cout << "Deleting scene's save files from " << fileName << std::endl;

		if (b_default_file_exists)
		{
			if (remove(default_save_file_path.c_str()) !=
			    0)
				std::cerr << "Error deleting default save file: " << default_save_file_path << std::endl;
			else std::cout << "Default save file deleted successfully." << std::endl;
		}

		if (b_saved_file_exists)
		{
			if (remove(saved_save_file_path.c_str()) !=
			    0)
				std::cerr << "Error deleting saved save file: " << saved_save_file_path << std::endl;
			else std::cout << "Saved save file deleted successfully." << std::endl;
		}
	}
}

/**
 * @brief Vérifie si un élément est contenu dans un vecteur.
 * @param _container Vecteur dans lequel chercher.
 * @param _value Valeur à chercher.
 * @return true si la valeur est trouvée dans le vecteur, sinon false.
 */
bool BaseScene::Contains(const std::vector<GameObject::id_t>& _container, const GameObject::id_t& _value)
{
	return std::find(_container.begin(), _container.end(), _value) != _container.end();
}

/**
 * @brief Supprime un objet de jeu de la scène.
 * @param _gameObjectId Identifiant de l'objet de jeu à supprimer.
 * @param _bDestroy Indique s'il faut détruire l'objet de jeu.
 */
void BaseScene::RemoveObject(const GameObject::id_t& _gameObjectId, bool _bDestroy)
{
	if (_bDestroy)
	{
		if (!Contains(pendingDestroyObjects, _gameObjectId)) pendingDestroyObjects.push_back(_gameObjectId);
	}
	else
	{
		if (!Contains(pendingRemoveObjects, _gameObjectId)) pendingRemoveObjects.push_back(_gameObjectId);
	}
}

/**
 * @brief Supprime un objet de jeu de la scène.
 * @param _gameObject Pointeur vers l'objet de jeu à supprimer.
 * @param _bDestroy Indique s'il faut détruire l'objet de jeu.
 */
void BaseScene::RemoveObject(const GameObject* _gameObject, const bool _bDestroy)
{
	RemoveObject(_gameObject->GetId(), _bDestroy);
}


/**
 * @brief Supprime plusieurs objets de jeu de la scène.
 * @param _gameObjects Vecteur contenant les identifiants des objets de jeu à supprimer.
 * @param _bDestroy Indique s'il faut détruire les objets de jeu.
 */
void BaseScene::RemoveObjects(const std::vector<GameObject::id_t>& _gameObjects, const bool _bDestroy)
{
	if (_bDestroy) pendingDestroyObjects.insert(pendingDestroyObjects.end(), _gameObjects.begin(), _gameObjects.end());
	else pendingRemoveObjects.insert(pendingRemoveObjects.end(), _gameObjects.begin(), _gameObjects.end());
}

/**
 * @brief Supprime plusieurs objets de jeu de la scène.
 * @param _gameObjects Vecteur contenant les pointeurs vers les objets de jeu à supprimer.
 * @param _bDestroy Indique s'il faut détruire les objets de jeu.
 */
void BaseScene::RemoveObjects(const std::vector<GameObject*>& _gameObjects, bool _bDestroy)
{
	if (_bDestroy)
	{
		pendingDestroyObjects.reserve(pendingDestroyObjects.size() + _gameObjects.size());
		for (const GameObject* game_object : _gameObjects)
		{
			if (!Contains(pendingDestroyObjects, game_object->GetId()))
				pendingDestroyObjects.push_back(
					game_object->GetId());
		}
	}
	else
	{
		pendingRemoveObjects.reserve(pendingRemoveObjects.size() + _gameObjects.size());
		for (const GameObject* game_object : _gameObjects)
		{
			if (!Contains(pendingRemoveObjects, game_object->GetId()))
				pendingRemoveObjects.push_back(
					game_object->GetId());
		}
	}
}


/**
 * @brief Crée un nouvel objet de jeu.
 * @return Pointeur vers le nouvel objet de jeu créé.
 */
GameObject* BaseScene::CreateGameObject()
{
	const auto game_object = new GameObject();
	pendingAddObjects.push_back(game_object);
	return game_object;
}

/**
 * @brief Détruit un objet de jeu.
 * @param _gameObject Pointeur vers l'objet de jeu à détruire.
 */

void BaseScene::DestroyGameObject(const GameObject* _gameObject)
{
	if (_gameObject != nullptr) RemoveObject(_gameObject, true);
}

/**
 * @brief Récupère un objet de jeu par son identifiant.
 * @param _gameObjectId Identifiant de l'objet de jeu à récupérer.
 * @return Pointeur vers l'objet de jeu correspondant à l'identifiant.
 */
GameObject* BaseScene::GetGameObjectById(const GameObject::id_t& _gameObjectId) const
{
	for (GameObject* root_object : rootObjects)
	{
		if (root_object->GetId() == _gameObjectId) return root_object;
	}
	return nullptr;
}

/**
 * @brief Recherche des objets de jeu par leur nom.
 * @param _name Nom des objets de jeu à rechercher.
 * @return Vecteur contenant les pointeurs vers les objets de jeu trouvés.
 */
std::vector<GameObject*> BaseScene::FindGameObjectsByName(const std::string& name) {
	std::vector<GameObject*> result;
	for (GameObject* rootObject : rootObjects) {
		std::vector<GameObject*> found = rootObject->FindChildrenByName(name);
		result.insert(result.end(), found.begin(), found.end());
	}
	return result;
}


void BaseScene::Init()
{
}

void BaseScene::Start()
{
	
}

void BaseScene::FixedUpdate(const float& _deltaTime)
{
}

/**
 * @brief Met à jour la scène.
 * @param _deltaTime Temps écoulé depuis la dernière mise à jour.
 */
void BaseScene::Update(const float& _deltaTime)
{
	// Mettez à jour chaque objet de la scène avec le delta time
	for (const GameObject* root_object : rootObjects)
	{
		root_object->Update(_deltaTime); // Mettez à jour chaque objet avec le delta time
	}
}

void BaseScene::PreRender()
{
}

/**
 * @brief Effectue le rendu de la scène.
 * @param _lveWindow Fenêtre de rendu.
 */
void BaseScene::Render(lve::LveWindow* _lveWindow)
{
	// Rendu de chaque objet de la scène
	for (GameObject* root_object : rootObjects)
	{
		//RenderObject(rootObject);
	}
}

void BaseScene::RenderGui()
{
}

void BaseScene::PostRender()
{
}

void BaseScene::Release()
{
}

/**
 * @brief Destructeur de la scène.
 * Réinitialise les indicateurs de chargement et d'initialisation, et détruit tous les objets racine de la scène.
 */
void BaseScene::Finalize()
{
	bLoaded      = false;
	bInitialized = false;

	for (GameObject* root_object : rootObjects)
	{
		if (root_object != nullptr)
		{
			for (Component* component : root_object->GetComponents())
			{
				root_object->RemoveComponent(component);
			}
			delete root_object;
		}
	}
	rootObjects.clear();
}


void BaseScene::TestLoadGameObjects()
{
	lve::LveDevice* _p_lveDevice = Engine::GetInstance().GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*_p_lveDevice, "Models\\flat_vase.obj");

	auto flatVaseGO = GameObject::CreatePGameObject();
	flatVaseGO->model = lve_model;
	flatVaseGO->GetTransform()->SetPosition(glm::vec3{ -.5f, .5f, 0.f });
	flatVaseGO->GetTransform()->SetScale(glm::vec3{ 3.f, 1.5f, 3.f });
	gameObjects.push_back(flatVaseGO);



	lve_model = lve::LveModel::CreateModelFromFile(*_p_lveDevice, "Models\\smooth_vase.obj");
	auto smooth_vase_go = GameObject::CreatePGameObject();
	smooth_vase_go->model = lve_model;
	smooth_vase_go->GetTransform()->SetPosition(glm::vec3{ .5f, .5f, 0.f });
	smooth_vase_go->GetTransform()->SetScale(glm::vec3{ 3.f, 1.5f, 3.f });
	gameObjects.push_back(smooth_vase_go);

	auto quad_go = lve::PlaneGameObject::Creates(*_p_lveDevice, glm::vec3{ .0f, .5f, 0.f }, glm::vec3{ 3.f, 1.f, 3.f });
	gameObjects.push_back(quad_go);

	lve_model = lve::LveModel::CreateModelFromFile(*_p_lveDevice, "Models\\viking_room.obj");
	auto viking = GameObject::CreatePGameObject();
	viking->model = lve_model;
	viking->GetTransform()->SetPosition(glm::vec3{ 0.f, 0.f, 5.f });
	viking->GetTransform()->SetScale(glm::vec3{ 3.f, 3.f, 3.f });
	viking->GetTransform()->SetRotation(glm::vec3{ glm::radians(90.0f), glm::radians(90.0f), 0.0f });
	gameObjects.push_back(viking);

	auto cube = lve::CubeGameObject::Creates(*_p_lveDevice);
	gameObjects.push_back(cube);

	auto color_cube = lve::CubeGameObject::CreateColors(*_p_lveDevice, glm::vec3{ 0.f, 0.f, 10.f });
	gameObjects.push_back(color_cube);

	std::vector<glm::vec3> light_colors{
		{1.f, .1f, .1f},
		{.1f, .1f, 1.f},
		{.1f, 1.f, .1f},
		{1.f, 1.f, .1f},
		{.1f, 1.f, 1.f},
		{1.f, 1.f, 1.f}
	};

	for (int i = 0; i < light_colors.size(); i++)
	{
		auto point_light = lve::LightGameObject::Creates(0.2f, 0.1f);
		point_light->color = light_colors[i];
		auto rotate_light = rotate(
			glm::mat4(1.f),
			(i * glm::two_pi<float>()) / light_colors.size(),
			{ 0.f, -1.f, 0.f });
		point_light->GetTransform()->SetPosition(glm::vec3(rotate_light * glm::vec4(-1.f, -1.f, -1.f, 1.f)));
		gameObjects.push_back(point_light);
	}

	auto sun = lve::LightGameObject::Creates(1000000.f, 2.0f, glm::vec3{ 0.f, -1000.f, 0.f });
	gameObjects.push_back(sun);
}