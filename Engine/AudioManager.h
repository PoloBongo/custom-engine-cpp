#pragma once
#include "GameObject.h"

#include <iostream>
#include <vector>

#include <fmod.hpp>
#include <fmod_errors.h>

class AudioManager
{
public:
	AudioManager();

	~AudioManager();

	void addAudios(GameObject* gameObject)
	{
		audios.push_back(gameObject);
	}

	void removeAudios(GameObject* gameObject); // enlever l' audio ciblé

	void addMusic(GameObject* gameObject)
	{
		musics.push_back(gameObject);
	}

	void removeMusics(GameObject* gameObject); // enlever la music ciblé

	void Update();

	float GetCurrentVolume() { return volume; }
	float GetMaxVolume() { return maxVolume; }
	void SetVolume(float _volume);

	void Pause(const std::string& _key);
	void Stop(const std::string& _key);

	void PlayAudio(const std::string& _key);
	void PlayMusic(const std::string& _key);

	void DeleteAudio(const std::string& _key);
	void DeleteMusic(const std::string& _key);

	void CreateSound();

private:
	std::vector<GameObject*> audios;
	std::vector<GameObject*> musics;
	float volume;
	float maxVolume;

	// FMOD PART
	FMOD_SYSTEM* sys;
	FMOD_SOUND* sound;
	FMOD_CHANNEL* channel;
	FMOD_CHANNELGROUP* channelGroup;
	FMOD_RESULT result;
};