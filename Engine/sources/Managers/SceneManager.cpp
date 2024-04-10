#include "Managers/SceneManager.h"
#include "Managers/WindowManager.h"
#include "ModuleManager.h"
#include <fstream>
#include <sstream>


/**
 * @brief Démarre le gestionnaire de scènes.
 * Initialise le gestionnaire de fenêtres.
 */
void SceneManager::Start() {
	Module::Start();

	windowManager = moduleManager->GetModule<WindowManager>();

	mainScene = CreateScene("Default", true);
}

/**
 * @brief Crée une nouvelle scène.
 * @param _name Nom de la nouvelle scène.
 * @param _isActive Indique si la nouvelle scène est active.
 */
BaseScene* SceneManager::CreateScene(std::string _name, bool _isActive)
{
	BaseScene* newScene = new BaseScene(_name);
	scenes.push_back(newScene);

	return newScene;
	//listScenes.insert(std::make_pair(_name, _isActive));
}

/**
 * @brief Récupère le nombre de scènes.
 * @return Le nombre de scènes.
 */
int SceneManager::SceneCount()
{
	int count = 0;
	if (!SceneManager::listScenes.empty())
	{
		count = SceneManager::listScenes.size();
	}
	return count;
}

/**
 * @brief Récupère le nom de la scène active.
 * @return Le nom de la scène active, ou "None" si aucune scène n'est active.
 */
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

/**
 * @brief Récupère le nom de la première scène dans la liste des scènes.
 * @return Le nom de la première scène, ou "None" si la liste des scènes est vide.
 */
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

/**
 * @brief Récupère la scène à un index donné dans la liste des scènes.
 * @param _index Index de la scène à récupérer.
 * @return Une paire contenant le nom de la scène et son état d'activité, ou une paire "None" si l'index est invalide.
 */
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


/**
 * @brief Supprime une scène de la liste des scènes.
 * @param sceneName Nom de la scène à supprimer.
 */
void SceneManager::DestroyScene(const std::string& sceneName)
{
	auto it = listScenes.find(sceneName);
	if (it != listScenes.end())
	{
		listScenes.erase(it);
	}
}

/**
 * @brief Définit la scène courante par son index dans la liste des scènes.
 * @param sceneIndex Index de la scène à définir comme scène courante.
 */
void SceneManager::SetCurrentScene(int sceneIndex) {
	if (sceneIndex >= 0 && sceneIndex < static_cast<int>(scenes.size())) {
		currentSceneIndex = sceneIndex;
	}
}

/**
 * @brief Active la scène suivante dans la liste des scènes.
 * Si la liste des scènes n'est pas vide, l'index de la scène courante est mis à jour pour passer à la scène suivante circulairement.
 */
void SceneManager::SetNextSceneActive() {
	if (!scenes.empty()) {
		currentSceneIndex = (currentSceneIndex + 1) % scenes.size();
	}
}

/**
 * @brief Active la scène précédente dans la liste des scènes.
 * Si la liste des scènes n'est pas vide, l'index de la scène courante est mis à jour pour passer à la scène précédente circulairement.
 */
void SceneManager::SetPreviousSceneActive() {
	if (!scenes.empty()) {
		currentSceneIndex = (currentSceneIndex - 1 + scenes.size()) % scenes.size();
	}
}


/**
 * @brief Récupère la scène courante.
 * @return Un pointeur vers la scène courante si l'index de la scène courante est valide, sinon nullptr.
 */
BaseScene* SceneManager::GetCurrentScene() const {
	if (currentSceneIndex >= 0 && currentSceneIndex < static_cast<int>(scenes.size())) {
		return scenes[currentSceneIndex];
	}
	return nullptr;//(currentSceneIndex >= 0 && currentSceneIndex < static_cast<int>(scenes.size())) ? scenes[currentSceneIndex].get() : nullptr;
}

/**
 * @brief Détruit toutes les scènes et vide la liste des scènes.
 * L'index de la scène courante est réinitialisé à -1.
 */
void SceneManager::Destroy()
{
	if (currentSceneIndex >= 0 && currentSceneIndex < static_cast<int>(scenes.size()))
	{
		scenes[currentSceneIndex]->Destroy();
	}

	scenes.clear();
	currentSceneIndex = -1;
}

/**
 * @brief Vérifie si un fichier de scène existe dans la liste des scènes.
 * @param fileName Nom du fichier de scène à rechercher.
 * @return true si le fichier de scène existe, sinon false.
 */
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
static GameObject::id_t nextGameObjectID = 0;
GameObject* CreateGameObjectFromSceneData(const std::string& sceneObjectName) {

	GameObject* gameObject = new GameObject();

	return gameObject;
}

/**
 * @brief Charge une scène à partir d'un fichier.
 * @param fileName Nom du fichier de scène à charger.
 * @return true si la scène a été chargée avec succès, sinon false.
 */
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

/**
 * @brief Définit la scène principale à utiliser.
 * @param sceneName Nom de la scène à définir comme scène principale.
 */
void SceneManager::SetMainScene(const std::string& sceneName) {
	auto it = listScenes.find(sceneName);
	if (it != listScenes.end()) {
		currentSceneIndex = std::distance(listScenes.begin(), it);
	}
}


/**
 * @brief Récupère une scène par son nom.
 * @param sceneName Nom de la scène à récupérer.
 * @return Un pointeur vers la scène si elle existe, sinon nullptr.
 */
BaseScene* SceneManager::GetScene(const std::string& sceneName) {
	for (BaseScene* scene : scenes)
	{
		if (scene->GetName() == sceneName)
		{
			return scene;
		}
	}

	return nullptr;
}

/**
 * @brief Récupère la scène principale.
 * @return Un pointeur vers la scène principale.
 */
std::vector<BaseScene*>& SceneManager::GetScenes() {
	return scenes;
}

/**
 * @brief Renomme une scène dans la liste des scènes.
 * @param oldName Ancien nom de la scène à renommer.
 * @param newName Nouveau nom de la scène.
 */
void SceneManager::RenameScene(const std::string& oldName, const std::string& newName) {
	auto it = listScenes.find(oldName);
	if (it != listScenes.end()) {
		listScenes[newName] = it->second;
		listScenes.erase(it);
	}
}

/**
 * @brief Met à jour la scène principale.
 * Appelle la méthode Update de la scène principale avec un delta time fixe.
 */
void SceneManager::UpdateMainScene()
{
	float deltaTime = 0.016f;
	mainScene->Update(deltaTime); // Appel de la méthode Update de BaseScene
}

/**
 * @brief Effectue le rendu de la scène principale.
 * Appelle la méthode Render de la scène principale en passant le pointeur de la fenêtre.
 */
void SceneManager::RenderMainScene()
{
	mainScene->Render(windowManager->GetWindow()); // Passe le pointeur de la fenêtre à la fonction Render
}

void SceneManager::MarkGameObjectForDeletion(GameObject* _gameObject) {
	// Ajoute le GameObject à une liste de suppression
	objectsToDelete.push_back(_gameObject);
}

void SceneManager::DeleteMarkedGameObjects() {
	for (auto* gameObject : objectsToDelete) {
		// Supprime l'objet ici
		delete gameObject;
	}
	objectsToDelete.clear();
}