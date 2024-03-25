#include "SceneManager.h"

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