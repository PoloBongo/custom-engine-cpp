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
void      GameObject::SetPosition(const glm::vec3 _newPosition) const { transform->SetPosition(_newPosition); }

glm::vec3 GameObject::GetScale() const { return transform->GetScale(); }
void      GameObject::SetScale(const glm::vec3 _newScale) const { transform->SetScale(_newScale); }

glm::vec3 GameObject::GetRotation() const { return transform->GetRotation(); }
void  GameObject::SetRotation(const float _newRotation) const { transform->SetRotation(_newRotation); }

void GameObject::AddComponent(Component* _component)
{
	_component->SetOwner(this);
	components.push_back(_component);
}

void GameObject::RemoveComponent(Component* _component)
{
	std::erase(components, _component);
	delete _component;
}

void GameObject::Init()
{
}

void GameObject::Start()
{
	for (Component* const& component : components)
	{
		component->Start();
	}
}

void GameObject::FixedUpdate(const float& _deltaTime) const
{
	if (this->isActive)
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetActive()) components[i]->FixedUpdate();
		}
}

void GameObject::Update(const float& _deltaTime) const
{
	if (this->isActive)
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetActive()) components[i]->Update();
		}
}

void GameObject::PreRender()
{
}

void GameObject::Render(lve::LveWindow* _window) const
{
	/*if (this->isVisible)
		for (const auto component : components)
		{
			if (component->GetVisible())
			{
				if (layerType == LayerType::Normal) component->Render(_window);
				else if (layerType == LayerType::HUD) component->RenderGui(_window);
				else if (layerType == LayerType::Background) component->RenderBackground(_window);
			}
		}*/
}

void GameObject::RenderGui()
{
}

void GameObject::PostRender()
{
}

void GameObject::Release()
{
}

void GameObject::Finalize()
{
}


std::vector<GameObject*> GameObject::FindChildrenByName(const std::string& _name) 
{
	std::vector<GameObject*> found_objects;

	// Parcourir tous les enfants du GameObject
	for (GameObject* child : children)
	{
		// V�rifier si le nom du GameObject correspond au nom recherch�
		if (child->GetName() == _name)
			// Ajouter le GameObject � la liste des objets trouv�s
			found_objects.push_back(child);

		// R�cursivement chercher les enfants du GameObject actuel
		std::vector<GameObject*> nested_found = child->FindChildrenByName(_name);
		found_objects.insert(found_objects.end(), nested_found.begin(), nested_found.end());
	}

	return found_objects;
}

void GameObject::SetParent(GameObject* _parent) {
	parent = _parent;
}

void GameObject::AddChildObject(GameObject* child) {
	children.push_back(child);
	child->SetParent(this);  

	glm::vec3 parentPosition = this->GetPosition();
	glm::vec3 childPosition = child->GetPosition();
	glm::vec3 relativePosition = childPosition - parentPosition;
	child->SetPosition(relativePosition);
}


Component* GameObject::GetComponentRecursive(const std::string& componentName) {
	// V�rifier si le GameObject a le composant sp�cifi�
	for (Component* component : components) {
		if (component->GetName() == componentName) {
			return component;
		}
	}

	// Recherche r�cursive dans les enfants
	for (GameObject* child : children) {
		Component* foundComponent = child->GetComponentRecursive(componentName);
		if (foundComponent != nullptr) {
			return foundComponent;
		}
	}

	// Recherche r�cursive dans le parent
	if (parent != nullptr) {
		return parent->GetComponentRecursive(componentName);
	}

	// Le composant n'a pas �t� trouv� dans ce GameObject ou ses enfants
	return nullptr;
}