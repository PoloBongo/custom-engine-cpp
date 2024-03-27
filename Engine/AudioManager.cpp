#include "AudioManager.h"


AudioManager::AudioManager()
{
	std::cout << "instance audio manager init" << std::endl;
	FMOD_System_Create(&sys, 0);
	FMOD_System_Init(sys, 4, FMOD_INIT_NORMAL, NULL);
}

AudioManager::~AudioManager()
{
	std::cout << "instance audio manager destroy" << std::endl;
	//FMOD_System_Release(sys);
}

void AudioManager::CreateSound()
{
	std::cout << "audio create" << std::endl;
	result = FMOD_System_CreateStream(sys, "test.wav", FMOD_LOOP_NORMAL, 0, &sound);
	if (result != FMOD_OK)
	{
		std::cout << "Fichier non charge" << std::endl;
	}
	FMOD_System_PlaySound(sys, sound, channelGroup, 0, &channel);
}