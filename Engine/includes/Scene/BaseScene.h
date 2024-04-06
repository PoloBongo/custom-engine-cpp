#pragma once

#include <string>
#include "lve_window.h"
#include "GameObject/GameObject.h"

class BaseScene
{
	public:
		explicit BaseScene(const std::string& _fileName);
		~BaseScene() = default;

		BaseScene(const BaseScene&)            = delete;
		BaseScene& operator=(const BaseScene&) = delete;

		/**
				* @brief Initialise le module.
				*/
		virtual void Init();

		/**
		 * @brief Démarre le module.
		 */
		virtual void Start();

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		virtual void FixedUpdate(const float& _deltaTime);

		/**
		 * @brief Met à jour le module.
		 */
		virtual void Update(const float& _deltaTime);

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		virtual void PreRender();

		/**
		 * @brief Rendu du module.
		 */
		virtual void Render(lve::LveWindow* _lveWindow);

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		virtual void RenderGui();

		/**
		 * @brief Fonction post-rendu du module.
		 */
		virtual void PostRender();

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		virtual void Release();

		/**
		 * @brief Finalise le module.
		 */
		virtual void Finalize();


		GameObject*              CreateGameObject();
		void                     DestroyGameObject(GameObject* _gameObject);
		GameObject*              GetGameObjectById(const GameObject::id_t& _gameObjectId);
		std::vector<GameObject*> FindGameObjectsByName(const std::string& _name);


		bool IsInitialized() const;

		void        SetName(const std::string& _name);
		std::string GetName() const;
		std::string GetDefaultRelativeFilePath() const;
		auto        SetFileName(const std::string& _fileName, bool _bDeletePreviousFiles) const -> bool;
		bool FileExists(const std::string& _filePath);
		bool IsUsingSaveFile() const;

		void DeleteSaveFiles();

		std::string GetFileName() const;


		std::vector<GameObject*>& GetRootObjects();

		GameObject* AddRootObject(GameObject* _gameObject);
		GameObject* AddRootObjectImmediate(GameObject* _gameObject);
		GameObject* AddChildObject(GameObject* _parent, GameObject* _child);
		GameObject* AddChildObjectImmediate(GameObject* _parent, GameObject* _child);
		GameObject* AddSiblingObjectImmediate(GameObject* _gameObject, GameObject* _newSibling);

		void SetRootObjectIndex(GameObject* _rootObject, uint32_t _newIndex);


		void RemoveAllObjects();          // Removes and destroys all objects in scene at end of frame
		void RemoveAllObjectsImmediate(); // Removes and destroys all objects in scene
		void RemoveAllEditorObjectsImmediate();
		void RemoveObject(const GameObject::id_t& _gameObjectId, bool _bDestroy);
		void RemoveObject(GameObject* _gameObject, bool _bDestroy);
		//void RemoveObjectImmediate(const GameObject::id_t& _gameObjectID, bool _bDestroy);
		//void RemoveObjectImmediate(GameObject* _gameObject, bool _bDestroy);
		void RemoveObjects(const std::vector<GameObject::id_t>& _gameObjects, bool _bDestroy);
		void RemoveObjects(const std::vector<GameObject*>& _gameObjects, bool _bDestroy);
		void RemoveObjectsImmediate(const std::vector<GameObject::id_t>& _gameObjects, bool _bDestroy);
		void RemoveObjectsImmediate(const std::vector<GameObject*>& _gameObjects, bool _bDestroy);

		GameObject::id_t FirstObjectWithTag(const std::string& _tag);

		std::string name;
		std::string fileName;

		std::vector<GameObject::id_t> pendingDestroyObjects; // Objects to destroy at LateUpdate this frame
		std::vector<GameObject*>      pendingAddObjects;     // Objects to add as root objects at LateUpdate
		std::vector<GameObject*>      rootObjects;
		std::vector<GameObject::id_t> pendingRemoveObjects;

		const GameObject::id_t invalidGameObjectId = {};


		bool bInitialized = false;
		bool bLoaded      = false;
};
