#pragma once

#include <glm.hpp>
#include <string>
#include <vector>
#include "Component.h"
#include "lve_window.h"

enum class LayerType
{
	Normal,
	HUD,
	Background
};

class Component;

class Transform;

class GameObject
{
	public:
		using id_t = unsigned int;
		GameObject();

		GameObject(id_t _id) : id(_id)
		{
		}

		~GameObject();

		std::string GetName() const { return name; }
		void        SetName(const std::string& _newName) { name = _newName; }

		Transform* GetTransform() const;

		glm::vec3 GetPosition() const;
		void      SetPosition(glm::vec3 _newPosition);

		glm::vec3 GetScale() const;
		void      SetScale(glm::vec3 _newScale);

		float GetRotation() const;
		void  SetRotation(float _newRotation);

		void SetActive(const bool& _state) { isActive = _state; }
		bool GetActive() const { return isActive; }

		bool GetVisible() const { return isVisible; }
		void SetVisible(const bool& _state) { isVisible = _state; }

		void SetActiveAndVisible(const bool& _state)
		{
			isActive  = _state;
			isVisible = _state;
		}

		LayerType GetLayer() const { return layerType; }
		void      SetLayer(const LayerType& _layerType) { layerType = _layerType; }

		std::vector<GameObject*> FindChildrenByName(const std::string& name);
		std::vector<GameObject*> Children;

		const std::vector<GameObject*>& GetChildren() const
		{
			return Children;
		}

		id_t GetId() { return id; }

		static GameObject CreateGameObject()
		{
			static id_t currentId = 0;
			return GameObject{currentId++};
		}

		void AddComponent(Component* _component);

		template <typename T>
		T* CreateComponent()
		{
			T* component = new T();
			component->SetOwner(this);
			components.push_back(component);
			return component;
		}

		std::vector<Component*> GetComponents() { return components; }

		template <typename T>
		T* GetComponent()
		{
			for (size_t i = 0; i < components.size(); i++)
			{
				// Vérifie si le composant est un Component
				T* componentResult = dynamic_cast<T*>(components[i]);
				if (componentResult) return componentResult; // Renvoie le Component trouvé
			}
			return nullptr; // Renvoie nullptr si aucun Component n'est trouvé
		}

		template <typename T>
		std::vector<T*> GetComponentsByType()
		{
			std::vector<T*> componentsByType;
			for (size_t i = 0; i < components.size(); i++)
			{
				// Vérifie si le composant est un Component
				T* componentResult = dynamic_cast<T*>(components[i]);
				if (componentResult) componentsByType.push_back(componentResult); // Ajout le Component trouvé
			}
			return componentsByType;
		}

		template <typename T>
		T* GetComponentByName(const std::string& _name)
		{
			std::vector<T*> componentsByType = GetComponentsByType<T>();
			for (size_t i = 0; i < componentsByType.size(); i++)
			{
				// Vérifie si le composant est un Component
				T* componentResult = dynamic_cast<T*>(componentsByType[i]);
				if (componentResult && static_cast<Component*>(componentResult)->GetName() == _name)
					return
						componentResult; // Renvoie le Component trouvé
			}
			return nullptr; // Renvoie nullptr si aucun Component n'est trouvé
		}

		void RemoveComponent(Component* _component);

		virtual void Start();
		void         Physics(const float& _delta) const;
		void         Update(const float& _delta) const;
		void         Render(lve::LveWindow* _window) const;

	protected:
		std::string             name = "GameObject";
		std::vector<Component*> components;

		Transform* transform = nullptr;
		//Quel est le layer du gameObject
		LayerType layerType = LayerType::Normal;

		//Plus c'est proche de 1, plus le GameObject sera proche de l'écran
		float depth = 0.f;

		bool isActive  = true;
		bool isVisible = true;

		id_t id;
};
