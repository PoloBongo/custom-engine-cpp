#include "GameObject/GameObject.h"
#include "Transform.h"




GameObject::GameObject()
{
	this->isActive = true;
	this->transform = new Transform();
}

GameObject::~GameObject()
{

	for (const Component* component : components)
	{
		if (component) delete component;
	}
	components.clear();
}

Transform* GameObject::GetTransform() const { return transform; }

glm::vec3 GameObject::GetPosition() const { return transform->GetPosition(); }
void GameObject::SetPosition(glm::vec3 _newPosition) { transform->SetPosition(_newPosition); }

glm::vec3 GameObject::GetScale() const { return transform->GetScale(); }
void GameObject::SetScale(glm::vec3 _newScale) { transform->SetScale(_newScale); }

float GameObject::GetRotation() const { return transform->GetRotation(); }
void GameObject::SetRotation(float _newRotation) { transform->SetRotation(_newRotation); }

void GameObject::AddComponent(Component* _component)
{
	_component->SetOwner(this);
	components.push_back(_component);
}

void GameObject::RemoveComponent(Component* _component)
{

	components.erase(std::remove(components.begin(), components.end(), _component), components.end());
	delete _component;

}

void GameObject::Start()
{
	for (Component* const& component : components)
	{
		component->Start();
	}
}

void GameObject::Physics(const float& _delta) const
{
	if (this->isActive)
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetActive()) {
				components[i]->Physics(_delta);
			}
		}
	}
}

void GameObject::Update(const float& _delta) const
{
	if (this->isActive)
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetActive()) {
				components[i]->Update(_delta);
			}
		}
	}
}

void GameObject::Render(lve::LveWindow* _window) const
{
	if (this->isVisible) {
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetVisible())
			{
				if (layerType == LayerType::Normal)
				{
					components[i]->Render(_window);
				}
			else if (layerType == LayerType::HUD)
				{
					components[i]->RenderGUI(_window);
				}
				else if (layerType == LayerType::Background)
				{
					components[i]->RenderBackground(_window);
				}
			}
		}
	}
}


std::vector<GameObject*> GameObject::FindChildrenByName(const std::string& name)
{
	std::vector<GameObject*> foundObjects;

	// Parcourir tous les enfants du GameObject
	for (GameObject* child : Children)
	{
		// Vérifier si le nom du GameObject correspond au nom recherché
		if (child->GetName() == name)
		{
			// Ajouter le GameObject à la liste des objets trouvés
			foundObjects.push_back(child);
		}

		// Récursivement chercher les enfants du GameObject actuel
		std::vector<GameObject*> nestedFound = child->FindChildrenByName(name);
		foundObjects.insert(foundObjects.end(), nestedFound.begin(), nestedFound.end());
	}

	return foundObjects;
}

