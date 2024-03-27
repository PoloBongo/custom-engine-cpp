#pragma once
#include "GameObject/GameObject.h"

#include <vector>

class ColliderManager
{
public:
	ColliderManager() {}

	~ColliderManager() {}

	void addCollider(GameObject* gameObject)
	{
		collider.push_back(gameObject);
	}

	void removeCollider(GameObject* gameObject); // enlever le collider ciblé

	void Update();

private:
	std::vector<GameObject*> collider;
};