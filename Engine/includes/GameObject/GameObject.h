#pragma once

#include <glm.hpp>
#include <string>
#include <vector>
#include "Component.h"
#include "lve_game_object.h"
#include "lve_window.h"

namespace lve
{
	struct PointLightComponent;
	class LveModel;
}

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
	GameObject();
	using id_t = unsigned int;
	id_t GetId() const { return id; }

	/**
	* @brief Constructeur de GameObject avec ID spécifique.
	*
	* Initialise un GameObject avec un identifiant unique. Cela garantit que chaque GameObject
	* a un identifiant unique au sein de la scène.
	*
	* @param _id Identifiant unique du GameObject.
	*/
	GameObject(id_t _id);
	~GameObject();

	

	[[nodiscard]] std::string GetName() const { return name; }
	void                      SetName(const std::string& _newName) { name = _newName; }

	[[nodiscard]] Transform* GetTransform() const;

	[[nodiscard]] glm::vec3 GetPosition() const;
	void                    SetPosition(glm::vec3 _newPosition) const;

	[[nodiscard]] glm::vec3 GetScale() const;
	void                    SetScale(glm::vec3 _newScale) const;

	[[nodiscard]] glm::vec3 GetRotation() const;
	void                SetRotation(float _newRotation) const;

	void               SetActive(const bool& _state) { isActive = _state; }
	[[nodiscard]] bool GetActive() const { return isActive; }

	[[nodiscard]] bool GetVisible() const { return isVisible; }
	void               SetVisible(const bool& _state) { isVisible = _state; }

	void SetActiveAndVisible(const bool& _state)
	{
		isActive  = _state;
		isVisible = _state;
	}

	[[nodiscard]] LayerType GetLayer() const { return layerType; }
	void                    SetLayer(const LayerType& _layerType) { layerType = _layerType; }

	GameObject* parent = nullptr;
	void SetParent(GameObject* _parent);

	std::vector<GameObject*> children;
	std::vector<GameObject*> FindChildrenByName(const std::string& name);
	void AddChildObject(GameObject* child);

	Component* GetComponentRecursive(const std::string& componentName);

	const std::vector<GameObject*>& GetChildren() const
	{
		return children;
	}


	static GameObject CreateGameObject()
	{
		static id_t current_id = 0;
		return GameObject{current_id++};
	}

	static GameObject* CreatePGameObject()
	{ 
		static id_t current_id = 0;
		return new GameObject{ current_id++ };
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
			if (T* component_result = dynamic_cast<T*>(components[i])) return component_result;
			// Renvoie le Component trouvé
		}
		return nullptr; // Renvoie nullptr si aucun Component n'est trouvé
	}

	template <typename T>
	std::vector<T*> GetComponentsByType()
	{
		std::vector<T*> components_by_type;
		for (size_t i = 0; i < components.size(); i++)
		{
			// Vérifie si le composant est un Component
			if (T* component_result = dynamic_cast<T*>(components[i])) components_by_type.
				push_back(component_result); // Ajout le Component trouvé
		}
		return components_by_type;
	}

	template <typename T>
	T* GetComponentByName(const std::string& _name)
	{
		std::vector<T*> components_by_type = GetComponentsByType<T>();
		for (size_t i = 0; i < components_by_type.size(); i++)
		{
			// Vérifie si le composant est un Component
			T* component_result = dynamic_cast<T*>(components_by_type[i]);
			if (component_result && static_cast<Component*>(component_result)->GetName() == _name)
				return
					component_result; // Renvoie le Component trouvé
		}
		return nullptr; // Renvoie nullptr si aucun Component n'est trouvé
	}

	void RemoveComponent(Component* _component);

	/**
	* @brief Finalise le module.
	*/
	virtual void Init();

	/**
	* @brief Démarre le module.
	*/
	virtual void Start();

	/**
	* @brief Effectue une mise à jour fixe du module.
	*/
	virtual void FixedUpdate(const float& _deltaTime) const;

	/**
	* @brief Met à jour le module.
	*/
	virtual void Update(const float& _deltaTime) const;

	/**
	* @brief Fonction pré-rendu du module.
	*/
	virtual void PreRender();

	/**
	* @brief Rendu du module.
	*/
	virtual void Render() const;

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

	std::shared_ptr<lve::LveModel>            model{}; /**< Modèle de l'objet. */
	glm::vec3          color{};     /**< Couleur de l'objet. */
	int texture = 0;

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

	id_t id; // Identifiant unique du GameObject
};


enum class GameObjectType {
	Cube,
	Light,
	Plane
	// TODO: Ajouter plus de types d'objets
};