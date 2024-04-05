#include "GameObject/GameObject.h"
#include "Transform.h"


GameObject::GameObject()
{
	this->isActive  = true;
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

float GameObject::GetRotation() const { return transform->GetRotation(); }
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
			if (components[i]->GetActive()) components[i]->Physics(_deltaTime);
		}
}

void GameObject::Update(const float& _deltaTime) const
{
	if (this->isActive)
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->GetActive()) components[i]->Update(_deltaTime);
		}
}

void GameObject::PreRender()
{
	
}

void GameObject::Render(lve::LveWindow* _window) const
{
	if (this->isVisible)
		for (const auto component : components)
		{
			if (component->GetVisible())
			{
				if (layerType == LayerType::Normal) component->Render(_window);
				else if (layerType == LayerType::HUD) component->RenderGUI(_window);
				else if (layerType == LayerType::Background) component->RenderBackground(_window);
			}
		}
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


std::vector<GameObject*> GameObject::FindChildrenByName(const std::string& _name) const
{
	std::vector<GameObject*> found_objects;

	// Parcourir tous les enfants du GameObject
	for (GameObject* child : children)
	{
		// Vérifier si le nom du GameObject correspond au nom recherché
		if (child->GetName() == _name)
			// Ajouter le GameObject à la liste des objets trouvés
			found_objects.push_back(child);

		// Récursivement chercher les enfants du GameObject actuel
		std::vector<GameObject*> nestedFound = child->FindChildrenByName(_name);
		found_objects.insert(found_objects.end(), nestedFound.begin(), nestedFound.end());
	}

	return found_objects;
}
