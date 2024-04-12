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

		void RunScene(const std::string& _sceneName);

		void       SetMainScene(const std::string& _sceneName);
		BaseScene* GetScene(const std::string& _sceneName);
		void       RenameScene(const std::string& _oldName, const std::string& _newName);


		void CreateScene(std::string _name, bool _isActive);
		void DestroyScene(const std::string& _sceneName);

		bool LoadSceneFromFile(const std::string& _fileName);

		std::string GetActiveScene() const;

		std::string GetListScenes() const;

		int SceneCount() const;

		std::pair<std::string, bool> GetSceneAt(int _index);

		BaseScene* GetCurrentScene() const;
		std::vector<std::unique_ptr<BaseScene>>& GetScenes() { return scenes; }

		void SetCurrentScene(int _sceneIndex);
		void SetNextSceneActive();
		void SetPreviousSceneActive();


		/**
			* @brief Initialise le module.
			*/
		void Init() override;

		/**
		 * @brief Démarre le module.
		 */
		void Start() override;

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		void FixedUpdate() override;

		/**
		 * @brief Met à jour le module.
		 */
		void Update() override;

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		void PreRender() override;

		/**
		 * @brief Rendu du module.
		 */
		void Render() override;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui() override;

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender() override;

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		void Release() override;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() override;

		static SceneManager* GetInstance() {
			static SceneManager instance;
			return &instance;
		}
	private:
		bool        SceneFileExists(const std::string& _filePath) const;
		GameObject* CreateGameObjectFromSceneData();

		WindowModule*                           windowModule = nullptr;
		std::map<std::string, bool>             listScenes;
		int                                     sceneCount;
		bool                                    sceneActive;
		BaseScene*				                mainScene = nullptr;
		std::vector<std::unique_ptr<BaseScene>> scenes;
		int                                     currentSceneIndex = -1;
};
