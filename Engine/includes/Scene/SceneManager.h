#pragma once
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include "Modules/Module.h"
#include "Modules/WindowModule.h"
#include "Scene/BaseScene.h"

/**
 * @brief Gestionnaire de scène.
 *
 * Gère les opérations liées aux scènes, telles que la création, la destruction et la manipulation des objets de la scène.
 */
class SceneManager final : public Module
{
	public:
		SceneManager() = default;

		SceneManager(const SceneManager&&)            = delete;
		SceneManager(const SceneManager&)             = delete;
		SceneManager& operator=(const SceneManager&&) = delete;
		SceneManager& operator=(const SceneManager&)  = delete;


#pragma region Getter

		/**
		 * @brief Obtient la scène principale.
		 *
		 * @return Un pointeur vers la scène principale.
		 */
		[[nodiscard]] BaseScene* GetMainScene() const { return mainScene; }

		/**
		 * @brief Obtient une scène spécifiée par son nom.
		 *
		 * @param _sceneName Le nom de la scène à récupérer.
		 * @return Un pointeur vers la scène demandée, ou nullptr si elle n'est pas trouvée.
		 */
		BaseScene* GetScene(const std::string& _sceneName);

		/**
		 * @brief Obtient le nom de la scène active.
		 *
		 * @return Le nom de la scène active.
		 */
		std::string GetActiveScene() const;

		/**
		 * @brief Obtient une liste des noms de toutes les scènes disponibles.
		 *
		 * @return Une chaîne contenant les noms de toutes les scènes disponibles.
		 */
		std::string GetListScenes() const;

		/**
		 * @brief Obtient la scène à l'index spécifié.
		 *
		 * @param _index L'index de la scène à récupérer.
		 * @return Une paire contenant le nom de la scène et un booléen indiquant si elle est active.
		 */
		std::pair<std::string, bool> GetSceneAt(int _index);

		/**
		 * @brief Obtient la scène actuellement active.
		 *
		 * @return Un pointeur vers la scène actuellement active.
		 */
		BaseScene* GetCurrentScene() const;

		/**
		 * @brief Obtient une référence à la liste des scènes.
		 *
		 * @return Une référence à la liste des scènes.
		 */
		std::vector<std::unique_ptr<BaseScene>>& GetScenes() { return scenes; }

#pragma endregion

#pragma region Setter

		/**
		 * @brief Définit la scène principale.
		 *
		 * @param _sceneName Le nom de la scène principale.
		 */
		void       SetMainScene(const std::string& _sceneName);

		/**
			 * @brief Définit la scène à activer en fonction de son index dans la liste des scènes.
			 *
			 * @param _sceneIndex L'index de la scène à activer.
			 */
		void SetCurrentScene(int _sceneIndex);

		/**
		 * @brief Active la scène suivante dans la liste des scènes.
		 */
		void SetNextSceneActive();

		/**
		 * @brief Active la scène précédente dans la liste des scènes.
		 */
		void SetPreviousSceneActive();

#pragma endregion 

		/**
		 * @brief Exécute une scène spécifiée.
		 *
		 * @param _sceneName Le nom de la scène à exécuter.
		 */
		void RunScene(const std::string& _sceneName);


		/**
		 * @brief Renomme une scène.
		 *
		 * @param _oldName Le nom actuel de la scène.
		 * @param _newName Le nouveau nom de la scène.
		 */
		void       RenameScene(const std::string& _oldName, const std::string& _newName);

		/**
		 * @brief Crée une nouvelle scène avec le nom spécifié.
		 *
		 * @param _name Le nom de la nouvelle scène.
		 * @param _isActive Indique si la nouvelle scène doit être active ou non.
		 */
		void CreateScene(std::string _name, bool _isActive);

		/**
		 * @brief Détruit une scène spécifiée par son nom.
		 *
		 * @param _sceneName Le nom de la scène à détruire.
		 */
		void DestroyScene(const std::string& _sceneName);

		/**
		 * @brief Charge une scène à partir d'un fichier avec le nom spécifié.
		 *
		 * @param _fileName Le nom du fichier de la scène à charger.
		 * @return true si la scène a été chargée avec succès, sinon false.
		 */
		bool LoadSceneFromFile(const std::string& _fileName);

		/**
		 * @brief Obtient le nombre total de scènes.
		 *
		 * @return Le nombre total de scènes.
		 */
		int SceneCount() const;

#pragma region Event

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

#pragma endregion

	private:

		/**
		 * @brief Vérifie si le fichier de la scène existe.
		 *
		 * @param _filePath Le chemin du fichier de la scène.
		 * @return true si le fichier de la scène existe, sinon false.
		 */
		bool SceneFileExists(const std::string& _filePath) const;

		/**
		 * @brief Crée un GameObject à partir des données de la scène.
		 *
		 * @return Un pointeur vers le GameObject créé.
		 */
		GameObject* CreateGameObjectFromSceneData();

		WindowModule* windowModule = nullptr; /**< Le module de la fenêtre associé au SceneManager. */

		std::map<std::string, bool> listScenes; /**< La liste des noms de scènes avec leur état actif. */

		int sceneCount; /**< Le nombre total de scènes. */

		bool sceneActive; /**< Indique si une scène est active. */

		BaseScene* mainScene = nullptr; /**< La scène principale du SceneManager. */

		std::vector<std::unique_ptr<BaseScene>> scenes; /**< Les scènes gérées par le SceneManager. */

		int currentSceneIndex = -1; /**< L'index de la scène actuellement active. */

};
