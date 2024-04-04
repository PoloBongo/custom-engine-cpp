#include "Managers/SceneManager.h"
#include "Managers/WindowManager.h"
#include "ModuleManager.h"
#include <fstream>
#include <sstream>

void SceneManager::Start() {
	Module::Start();

	windowManager = moduleManager->GetModule<WindowManager>();
}

void SceneManager::CreateScene(std::string _name, bool _isActive)
{
	listScenes.insert(std::make_pair(_name, _isActive));
}

int SceneManager::SceneCount()
{
	int count = 0;
	if (!SceneManager::listScenes.empty())
	{
		count = SceneManager::listScenes.size();
	}
	return count;
}

std::string SceneManager::GetActiveScene()
{
	std::string _name;
	bool isActive;
	for (const auto scene : listScenes)
	{
		_name = scene.first;
		isActive = scene.second;
		if (isActive)
		{
			return _name;
		}
	}
	return "None";
}

std::string SceneManager::GetListScenes()
{
	std::string _name;
	for (const auto scene : listScenes)
	{
		_name = scene.first;
		return _name;
	}
	return "None";
}

std::pair<std::string, bool> SceneManager::GetSceneAt(int _index)
{
	if (!listScenes.empty())
	{
		auto findName = listScenes.begin();
		std::advance(findName, _index);
		return *findName;
	}
	return { "None", false };
}



void SceneManager::DestroyScene(const std::string& sceneName)
{
	auto it = listScenes.find(sceneName);
	if (it != listScenes.end())
	{
		listScenes.erase(it);
	}
}


void SceneManager::SetCurrentScene(int sceneIndex) {
	if (sceneIndex >= 0 && sceneIndex < static_cast<int>(scenes.size())) {
		currentSceneIndex = sceneIndex;
	}
}

void SceneManager::SetNextSceneActive() {
	if (!scenes.empty()) {
		currentSceneIndex = (currentSceneIndex + 1) % scenes.size();
	}
}

void SceneManager::SetPreviousSceneActive() {
	if (!scenes.empty()) {
		currentSceneIndex = (currentSceneIndex - 1 + scenes.size()) % scenes.size();
	}
}



BaseScene* SceneManager::GetCurrentScene() const {
	return (currentSceneIndex >= 0 && currentSceneIndex < static_cast<int>(scenes.size())) ? scenes[currentSceneIndex].get() : nullptr;
}

void SceneManager::Destroy()
{
	if (currentSceneIndex >= 0 && currentSceneIndex < static_cast<int>(scenes.size()))
	{
		scenes[currentSceneIndex]->Destroy();
	}

	scenes.clear();
	currentSceneIndex = -1;
}

bool SceneManager::SceneFileExists(const std::string& fileName) const
{
	for (const auto& scene : scenes)
	{
		if (scene->GetFileName() == fileName)
		{
			return true;
		}
	}

	return false;
}

//--------------------------A MODIFIER TRES SIMPLIFIE------------------------------------
GameObject* CreateGameObjectFromSceneData(const std::string& sceneObjectName) {

	GameObject* gameObject = new GameObject();

	return gameObject;
}

bool SceneManager::LoadSceneFromFile(const std::string& fileName) {
	// Vérifier si le fichier existe
	if (!SceneFileExists(fileName)) {
		std::cerr << "Erreur : Le fichier de scène '" << fileName << "' n'existe pas." << std::endl;
		return false;
	}

	// Ouvrir le fichier de scène en lecture
	std::ifstream file(fileName);
	if (!file.is_open()) {
		std::cerr << "Erreur : Impossible d'ouvrir le fichier de scène '" << fileName << "'." << std::endl;
		return false;
	}

	// Lire le contenu du fichier de scène
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string sceneData = buffer.str();

	// Fermer le fichier
	file.close();

	// Analyser les données de la scène
	std::istringstream sceneStream(sceneData);
	std::string sceneObjectName;
	while (sceneStream >> sceneObjectName) {
		GameObject* gameObject = CreateGameObjectFromSceneData(sceneObjectName);
		if (gameObject) {
			// Ajouter l'objet de scène à la scène courante
			GetCurrentScene()->AddRootObject(gameObject);
		}
		else {
			std::cerr << "Erreur : Impossible de créer l'objet de scène '" << sceneObjectName << "'." << std::endl;
		}
	}

	std::cout << "Chargement de la scène depuis le fichier '" << fileName << "' terminé." << std::endl;
	return true;
}


void SceneManager::SetMainScene(const std::string& sceneName) {
	auto it = listScenes.find(sceneName);
	if (it != listScenes.end()) {
		currentSceneIndex = std::distance(listScenes.begin(), it);
	}
}



BaseScene* SceneManager::GetScene(const std::string& sceneName) {
	auto it = listScenes.find(sceneName);
	if (it != listScenes.end()) {
		return scenes[std::distance(listScenes.begin(), it)].get();
	}
	return nullptr;
}

void SceneManager::RenameScene(const std::string& oldName, const std::string& newName) {
	auto it = listScenes.find(oldName);
	if (it != listScenes.end()) {
		listScenes[newName] = it->second;
		listScenes.erase(it);
	}
}

void SceneManager::UpdateMainScene()
{
	float deltaTime = 0.016f;
	mainScene->Update(deltaTime); // Appel de la méthode Update de BaseScene
}

void SceneManager::RenderMainScene()
{
	mainScene->Render(windowManager->GetWindow()); // Passer le pointeur de la fenêtre à la fonction Render
}