#include "Managers/SceneManager.h"

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



//bool SceneManager::SceneFileExists(const std::string& filePath) const {
//	return std::any_of(scenes.begin(), scenes.end(), [&filePath](const auto& scene) {
//		return scene->GetFileName() == filePath;
//		});
//}