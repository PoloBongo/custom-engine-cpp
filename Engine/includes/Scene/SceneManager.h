#pragma once
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include "Modules/Module.h"
#include "Modules/WindowModule.h"
#include "Scene/BaseScene.h"


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
		void       UpdateMainScene() const;
		void       RenderMainScene() const;

		void Destroy();

		void CreateScene(std::string _name, bool _isActive);
		void DestroyScene(const std::string& _sceneName);

		bool LoadSceneFromFile(const std::string& _fileName);

		std::string GetActiveScene() const;

		std::string GetListScenes() const;

		int SceneCount() const;

		std::pair<std::string, bool> GetSceneAt(int _index);

		BaseScene* GetCurrentScene() const;

		void SetCurrentScene(int _sceneIndex);
		void SetNextSceneActive();
		void SetPreviousSceneActive();

	private:
		bool        SceneFileExists(const std::string& _filePath) const;
		GameObject* CreateGameObjectFromSceneData();

		WindowModule*                           windowModule = nullptr;
		std::map<std::string, bool>             listScenes;
		int                                     sceneCount;
		bool                                    sceneActive;
		BaseScene*                              mainScene = nullptr;
		std::vector<std::unique_ptr<BaseScene>> scenes;
		int                                     currentSceneIndex = -1;
};
