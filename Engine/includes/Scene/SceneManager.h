#pragma once
#include <iostream>
#include <string>
#include <map>
#include "Scene/BaseScene.h"
#include "Modules/WindowModule.h"
#include <filesystem>
#include "Modules/Module.h"


class SceneManager final : public Module
{
	public:
		SceneManager() = default;

		SceneManager(const SceneManager&&)            = delete;
		SceneManager(const SceneManager&)             = delete;
		SceneManager& operator=(const SceneManager&&) = delete;
		SceneManager& operator=(const SceneManager&)  = delete;

		void Start() override;

		void       SetMainScene(const std::string& _sceneName);
		BaseScene* GetScene(const std::string& _sceneName);
		void       RenameScene(const std::string& _oldName, const std::string& _newName);
		void       UpdateMainScene();
		void       RenderMainScene();

		void Destroy();

		void CreateScene(std::string _name, bool _isActive);
		void DestroyScene(const std::string& _sceneName);

		bool LoadSceneFromFile(const std::string& _fileName);

		std::string GetActiveScene();

		std::string GetListScenes();

		int SceneCount();

		std::pair<std::string, bool> GetSceneAt(int _index);

		BaseScene* GetCurrentScene() const;

		void SetCurrentScene(int _sceneIndex);
		void SetNextSceneActive();
		void SetPreviousSceneActive();

	private:
		WindowModule*                           windowModule = nullptr;
		std::map<std::string, bool>             listScenes;
		int                                     sceneCount;
		bool                                    sceneActive;
		BaseScene*                              mainScene = nullptr;
		bool                                    SceneFileExists(const std::string& _filePath) const;
		std::vector<std::unique_ptr<BaseScene>> scenes;
		int                                     currentSceneIndex = -1;
};
