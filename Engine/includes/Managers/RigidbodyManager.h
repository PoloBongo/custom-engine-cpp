#pragma once
#include "GameObject/GameObject.h"

#include <vector>

class RigidbodyManager
{
public:
	RigidbodyManager() {}

	~RigidbodyManager() {}

	void addRigidbody(GameObject* gameObject)
	{
		rigidbody.push_back(gameObject);
	}

	void removeRigidbody(GameObject* gameObject); // enlever le rigidbody ciblé

	void Update();

private:
	std::vector<GameObject*> rigidbody;
};

