#pragma once
#include "GameObject.h"

#include <vector>

class ColliderManager
{

private:
	std::vector<GameObject*> collider;

public:
	ColliderManager() {}

	~ColliderManager() {}

	void addCollider(GameObject* gameObject)
	{
		collider.push_back(gameObject);
	}

	void removeCollider(GameObject* gameObject) {} // enlever le collider ciblé

	void Update()
	{
		for (GameObject* gameObject : collider)
		{

		}
	}

};