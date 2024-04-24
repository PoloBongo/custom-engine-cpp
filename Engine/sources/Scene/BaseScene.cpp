#include "Scene/BaseScene.h"
#include <fstream>
#include <iostream>
#include <string>

#include "CoreEngine.h"
#include "rhi.h"
#include "GameObject/PreGameObject/CubeGameObject.h"
#include "GameObject/PreGameObject/LightGameObject.h"
#include "GameObject/PreGameObject/PlaneGameObject.h"

#include "Modules/TimeModule.h"


/**
 * @brief Ajoute un objet de jeu en tant qu'objet racine � la sc�ne.
 * @param _gameObject Pointeur vers l'objet de jeu � ajouter.
 * @return Pointeur vers l'objet de jeu ajout�.
 */
GameObject* BaseScene::AddRootObject(GameObject* _gameObject)
{
	if (_gameObject == nullptr) return nullptr;

	pendingAddObjects.push_back(_gameObject);

	return _gameObject;
}


/**
 * @brief Supprime tous les objets de la sc�ne � la fin du frame.
 */
void BaseScene::RemoveAllObjects()
{
	for (const GameObject* root_object : rootObjects)
	{
		pendingDestroyObjects.push_back(root_object->GetId());
	}
}

/**
 * @brief V�rifie si la sc�ne est initialis�e.
 * @return true si la sc�ne est initialis�e, sinon false.
 */
bool BaseScene::IsInitialized() const
{
	return bInitialized;
}

/**
 * @brief D�finit le nom de la sc�ne.
 * @param _name Nouveau nom de la sc�ne.
 */
void BaseScene::SetName(const std::string& _name)
{
	name = _name;
}

/**
 * @brief R�cup�re le nom de la sc�ne.
 * @return Le nom de la sc�ne.
 */
std::string BaseScene::GetName() const
{
	return name;
}

/**
 * @brief V�rifie si la sc�ne utilise un fichier de sauvegarde.
 * @return true si la sc�ne utilise un fichier de sauvegarde, sinon false.
 */
bool BaseScene::IsUsingSaveFile() const
{
	return false;
}

/**
 * @brief R�cup�re les objets racine de la sc�ne.
 * @return Une r�f�rence vers le vecteur contenant les objets racine de la sc�ne.
 */
std::vector<GameObject*>& BaseScene::GetRootObjects()
{
	return rootObjects;
}


/**
 * @brief R�cup�re le nom du fichier de la sc�ne.
 * @return Le nom du fichier de la sc�ne.
 */
std::string BaseScene::GetFileName() const
{
	return fileName;
}

/**
 * @brief D�finit le nom du fichier de la sc�ne.
 * @param _fileName Nouveau nom du fichier de la sc�ne.
 * @param _bDeletePreviousFiles Indique s'il faut supprimer les fichiers pr�c�dents.
 * @return true si le nom du fichier est d�fini avec succ�s, sinon false.
 */
bool BaseScene::SetFileName(const std::string& _fileName, bool _bDeletePreviousFiles) const
{
	bool success = false;

	if (fileName == _fileName) success = true;
	return success;
}

/**
 * @brief R�cup�re le chemin de fichier par d�faut de la sc�ne.
 * @return Le chemin de fichier par d�faut de la sc�ne.
 */
std::string BaseScene::GetDefaultRelativeFilePath() const
{
	//A MODIFIIIIEEEEEEEEEERRRRRR
	const std::string DEFAULT_SCENE_DIRECTORY = "assets/scenes/";
	return DEFAULT_SCENE_DIRECTORY + fileName;
}

/**
 * @brief V�rifie si un fichier existe.
 * @param _filePath Chemin du fichier � v�rifier.
 * @return true si le fichier existe, sinon false.
 */

bool BaseScene::FileExists(const std::string& _filePath)
{
	const std::ifstream file(_filePath);
	return file.good();
}

/**
 * @brief Supprime les fichiers de sauvegarde de la sc�ne.
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
 * @brief V�rifie si un �l�ment est contenu dans un vecteur.
 * @param _container Vecteur dans lequel chercher.
 * @param _value Valeur � chercher.
 * @return true si la valeur est trouv�e dans le vecteur, sinon false.
 */
bool BaseScene::Contains(const std::vector<GameObject::id_t>& _container, const GameObject::id_t& _value)
{
	return std::find(_container.begin(), _container.end(), _value) != _container.end();
}

/**
 * @brief Supprime un objet de jeu de la sc�ne.
 * @param _gameObjectId Identifiant de l'objet de jeu � supprimer.
 * @param _bDestroy Indique s'il faut d�truire l'objet de jeu.
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
 * @brief Supprime un objet de jeu de la sc�ne.
 * @param _gameObject Pointeur vers l'objet de jeu � supprimer.
 * @param _bDestroy Indique s'il faut d�truire l'objet de jeu.
 */
void BaseScene::RemoveObject(const GameObject* _gameObject, const bool _bDestroy)
{
	if (_gameObject) {
		auto it = std::find(rootObjects.begin(), rootObjects.end(), _gameObject);
		if (it != rootObjects.end()) {
			rootObjects.erase(it);
			//if (_bDestroy) {
			//	delete _gameObject;
			//}
		}
	}
	RemoveObject(_gameObject->GetId(), _bDestroy);
}


/**
 * @brief Supprime plusieurs objets de jeu de la sc�ne.
 * @param _gameObjects Vecteur contenant les identifiants des objets de jeu � supprimer.
 * @param _bDestroy Indique s'il faut d�truire les objets de jeu.
 */
void BaseScene::RemoveObjects(const std::vector<GameObject::id_t>& _gameObjects, const bool _bDestroy)
{
	if (_bDestroy) pendingDestroyObjects.insert(pendingDestroyObjects.end(), _gameObjects.begin(), _gameObjects.end());
	else pendingRemoveObjects.insert(pendingRemoveObjects.end(), _gameObjects.begin(), _gameObjects.end());
}

/**
 * @brief Supprime plusieurs objets de jeu de la sc�ne.
 * @param _gameObjects Vecteur contenant les pointeurs vers les objets de jeu � supprimer.
 * @param _bDestroy Indique s'il faut d�truire les objets de jeu.
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
 * @brief Cr�e un nouvel objet de jeu.
 * @return Pointeur vers le nouvel objet de jeu cr��.
 */
GameObject* BaseScene::CreateGameObject()
{
	const auto game_object = new GameObject();
	pendingAddObjects.push_back(game_object);
	return game_object;
}

/**
 * @brief D�truit un objet de jeu.
 * @param _gameObject Pointeur vers l'objet de jeu � d�truire.
 */

void BaseScene::DestroyGameObject(const GameObject* _gameObject)
{
	if (_gameObject != nullptr) RemoveObject(_gameObject, true);
}

/**
 * @brief R�cup�re un objet de jeu par son identifiant.
 * @param _gameObjectId Identifiant de l'objet de jeu � r�cup�rer.
 * @return Pointeur vers l'objet de jeu correspondant � l'identifiant.
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
 * @param _name Nom des objets de jeu � rechercher.
 * @return Vecteur contenant les pointeurs vers les objets de jeu trouv�s.
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
	TestLoadGameObjects();
}

void BaseScene::FixedUpdate()
{
}

/**
 * @brief Met � jour la sc�ne.
 * @param _deltaTime Temps �coul� depuis la derni�re mise � jour.
 */
void BaseScene::Update()
{
	// Ajoutez tous les objets en attente à la liste principale des objets de la scène
	for (auto* obj : pendingAddObjects) {
		rootObjects.push_back(obj);
	}
	pendingAddObjects.clear();

	// Mettez � jour chaque objet de la sc�ne avec le delta time
	for (const GameObject* root_object : rootObjects)
	{
		root_object->Update(TimeModule::GetDeltaTime()); // Mettez � jour chaque objet avec le delta time
	}
}

void BaseScene::PreRender()
{
}

/**
 * @brief Effectue le rendu de la sc�ne.
 * @param _lveWindow Fen�tre de rendu.
 */
void BaseScene::Render()
{
	// Rendu de chaque objet de la sc�ne
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
 * @brief Destructeur de la sc�ne.
 * R�initialise les indicateurs de chargement et d'initialisation, et d�truit tous les objets racine de la sc�ne.
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
	lve::LveDevice* _p_lveDevice = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	std::shared_ptr<lve::LveModel> lve_model = lve::LveModel::CreateModelFromFile(*_p_lveDevice, "Models\\flat_vase.obj");

	auto flatVaseGO = GameObject::CreatePGameObject();
	flatVaseGO->SetName("FlatVase");
	flatVaseGO->model = lve_model;
	flatVaseGO->GetTransform()->SetPosition(glm::vec3{ -.5f, .5f, 0.f });
	flatVaseGO->GetTransform()->SetScale(glm::vec3{ 3.f, 1.5f, 3.f });
	flatVaseGO->texture = 1;
	rootObjects.push_back(flatVaseGO);



	lve_model = lve::LveModel::CreateModelFromFile(*_p_lveDevice, "Models\\smooth_vase.obj");
	auto smooth_vase_go = GameObject::CreatePGameObject();
	smooth_vase_go->SetName("SmoothVase");
	smooth_vase_go->model = lve_model;
	smooth_vase_go->GetTransform()->SetPosition(glm::vec3{ .5f, .5f, 0.f });
	smooth_vase_go->GetTransform()->SetScale(glm::vec3{ 3.f, 1.5f, 3.f });
	rootObjects.push_back(smooth_vase_go);

	auto quad_go = lve::PlaneGameObject::Creates(*_p_lveDevice, glm::vec3{ .0f, .5f, 0.f }, glm::vec3{ 3.f, 1.f, 3.f });
	quad_go->SetName("QuadGo");
	quad_go->texture = 3;
	rootObjects.push_back(quad_go);

	lve_model = lve::LveModel::CreateModelFromFile(*_p_lveDevice, "Models\\viking_room.obj");
	auto viking = GameObject::CreatePGameObject();
	viking->SetName("Viking");
	viking->model = lve_model;
	viking->GetTransform()->SetPosition(glm::vec3{ 0.f, 0.f, 5.f });
	viking->GetTransform()->SetScale(glm::vec3{ 3.f, 3.f, 3.f });
	viking->GetTransform()->SetRotation(glm::vec3{ glm::radians(90.0f), glm::radians(90.0f), 0.0f });
	viking->texture = 2;
	rootObjects.push_back(viking);

	auto cube = lve::CubeGameObject::Creates(*_p_lveDevice);
	cube->SetName("Cube");
	rootObjects.push_back(cube);

	auto color_cube = lve::CubeGameObject::CreateColors(*_p_lveDevice, glm::vec3{ 0.f, 0.f, 10.f });
	color_cube->SetName("ColorCube");
	rootObjects.push_back(color_cube);

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
		point_light->SetName("Point Light " + std::to_string(i));
		point_light->color = light_colors[i];
		auto rotate_light = rotate(
			glm::mat4(1.f),
			(i * glm::two_pi<float>()) / light_colors.size(),
			{ 0.f, -1.f, 0.f });
		point_light->GetTransform()->SetPosition(glm::vec3(rotate_light * glm::vec4(-1.f, -1.f, -1.f, 1.f)));
		rootObjects.push_back(point_light);
	}

	auto sun = lve::LightGameObject::Creates(1000000.f, 2.0f, glm::vec3{ 0.f, -1000.f, 0.f });
	sun->SetName("Sun");
	rootObjects.push_back(sun);
}

GameObject* BaseScene::CreateCubeGameObject() {
	lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	return lve::CubeGameObject::Creates(*device);
}

GameObject* BaseScene::CreateLightGameObject() {
	float intensity = 10.0f;  // Exemple d'intensité pour la lumière
	float radius = 1.0f;      // Rayon de la lumière
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);  // Position initiale
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);  // Rotation initiale
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);     // Couleur de la lumière (blanche)

	// Crée un GameObject représentant une lumière avec les paramètres spécifiés
	return lve::LightGameObject::Creates(intensity, radius, position, rotation, color);
}



GameObject* BaseScene::CreatePlaneGameObject() {
	lve::LveDevice* device = Engine::GetInstance()->GetModuleManager()->GetModule<RHIModule>()->GetDevice();
	return lve::PlaneGameObject::Creates(*device);
}
