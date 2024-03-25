#pragma once
#include "GameObject.h"

#include <vector>

class AudioManager
{
public:
	AudioManager() {}

	~AudioManager() {}

	void addAudio(GameObject* gameObject)
	{
		audio.push_back(gameObject);
	}

	void removeAudio(GameObject* gameObject) {} // enlever le audio ciblé

	void Update()
	{
		for (GameObject* gameObject : audio)
		{

		}
	}

private:
	std::vector<GameObject*> audio;
};