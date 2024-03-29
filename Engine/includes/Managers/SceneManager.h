#pragma once
#include <iostream>
#include <string>
#include <map>
#include "Scene/BaseScene.h"
#include <filesystem>

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
	void DestroyScene(const std::string& sceneName);


	std::string GetActiveScene();

	std::string GetListScenes();

	int SceneCount();
	
	std::pair<std::string, bool> GetSceneAt(int _index);

	BaseScene* GetCurrentScene() const;

	void SetCurrentScene(int sceneIndex);
	void SetNextSceneActive();
	void SetPreviousSceneActive();


private:
	std::map<std::string, bool> listScenes;
	int sceneCount;
	bool sceneActive;

	bool SceneFileExists(const std::string& filePath) const;
	std::vector<std::unique_ptr<BaseScene>> scenes;
	int currentSceneIndex = -1;
};


