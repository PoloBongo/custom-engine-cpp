#pragma once
#include <iostream>
#include <string>
#include <map>
#include "Scene/BaseScene.h"
#include "Managers/WindowManager.h"
#include <filesystem>
#include "Module.h"


class SceneManager final : public Module
{
public:
	SceneManager() = default;

	SceneManager(const SceneManager&&) = delete;
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	void Start() override;

	void SetMainScene(const std::string& sceneName);
	BaseScene* GetScene(const std::string& sceneName);
	BaseScene* GetMainScene() { return mainScene; }
	void RenameScene(const std::string& oldName, const std::string& newName);
	void UpdateMainScene();
	void RenderMainScene();

	void Destroy();

	BaseScene* CreateScene(std::string _name, bool _isActive);
	void DestroyScene(const std::string& sceneName);

	bool LoadSceneFromFile(const std::string& fileName);

	std::string GetActiveScene();

	std::string GetListScenes();

	int SceneCount();

	std::pair<std::string, bool> GetSceneAt(int _index);

	BaseScene* GetCurrentScene() const;

	void SetCurrentScene(int sceneIndex);
	void SetNextSceneActive();
	void SetPreviousSceneActive();


private:
	WindowManager* windowManager = nullptr;
	std::map<std::string, bool> listScenes;
	int sceneCount;
	bool sceneActive;
	BaseScene* mainScene = nullptr;
	bool SceneFileExists(const std::string& filePath) const;
	std::vector<BaseScene*> scenes;
	int currentSceneIndex = -1;
};


