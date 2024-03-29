#pragma once
#include <iostream>
#include <string>
#include <map>

class SceneManager
{
public:
	SceneManager() {};
	~SceneManager() = default;

	SceneManager(const SceneManager&&) = delete;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	void Destroy();

	void CreateScene(std::string _name, bool _isActive);

	void LoadScene(std::string _name);

	std::string GetActiveScene();

	std::string GetListScenes();

	int SceneCount();
	
	std::pair<std::string, bool> GetSceneAt(int _index);

private:
	std::map<std::string, bool> listScenes;
	int sceneCount;
	bool sceneActive;
};

