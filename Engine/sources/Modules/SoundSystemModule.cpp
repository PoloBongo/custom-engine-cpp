#include "Modules/SoundSystemModule.h"
#include <fmod_errors.h>
#include <string>

SoundSystemModule::SoundSystemModule()
{
	if (System_Create(&system) != FMOD_OK)
	{
		std::cout << "une erreur s'est produit lors de la création du Système de FMOD" << std::endl;
		return;
	}

	int driverCount = 0;
	system->getNumDrivers(&driverCount);

	if (driverCount == 0)
	{
		std::cout << "FMOD n'a pas trouvé de sortie de son" << std::endl;
		return;
	}

	// Permet d'init le nombre de musique joué simultanément
	system->init(36, FMOD_LOOP_OFF, nullptr);
}

// Permet la création du son avec son chemin d'accès //
void SoundSystemModule::createSound(SoundClass* pSound, const char* pathAudio)
{
	FMOD_RESULT result = system->createSound(pathAudio, FMOD_LOOP_OFF, nullptr, pSound);

	if (result != FMOD_OK) std::cout << "le son n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet la création d'un groupe de son. //
void SoundSystemModule::createSoundGroup(SoundGroup* pSoundGroup, const char* groupName)
{
	FMOD_RESULT result = system->createSoundGroup(groupName, pSoundGroup);

	if (result != FMOD_OK) std::cout << "le groupe n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet la récupération d'un groupe de son. //
void SoundSystemModule::getMasterSoundGroup(SoundGroup* pSoundGroup)
{
	FMOD_RESULT result = system->getMasterSoundGroup(pSoundGroup);

	if (result != FMOD_OK) std::cout << "le groupe n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet de joué un son spécifique avec ces propres paramètres. //
void SoundSystemModule::playSound(SoundClass pSound, bool isPlay, int loopCount, float volume, Channel* channelPtr)
{
	FMOD::Channel* channel = nullptr;

	if (!isPlay)
	{
		pSound->setMode(FMOD_LOOP_OFF);
	}
	else
	{
		pSound->setMode(FMOD_LOOP_NORMAL);
		pSound->setLoopCount(loopCount);
	}

	system->playSound(pSound, channelGroup, false, &channel);

	channel->setVolume(volume);

	// Affecter le canal créé au pointeur de pointeur passé en paramètre
	*channelPtr = channel;
}

// Permet de libérer la mémoire et de couper le son. //
void SoundSystemModule::releaseSound(SoundClass pSound)
{
	pSound->release();
}

// Permet d'obtenir le nombre total de tour sur le son //
int SoundSystemModule::getLoopCount(SoundClass pSound, int* loopcount)
{
	return pSound->getLoopCount(loopcount);
}

// Permet la création d'un channel de groupe. //
void SoundSystemModule::createChannelGroup(FMOD::ChannelGroup** channelGroup)
{
	system->createChannelGroup(nullptr, channelGroup);
}

// Permet d'ajouter un son à un channel de groupe pour pouvoir modifier les paramètres du son du tout les sons du channel. //
void SoundSystemModule::addSoundToGroup(SoundClass pSound, FMOD::ChannelGroup* channelGroup)
{
	system->playSound(pSound, channelGroup, false, nullptr);
}

// Implémentation des fonctions pour le routing vers les ports et la gestion de la réverbération //
FMOD_RESULT SoundSystemModule::attachChannelGroupToPort(FMOD_PORT_TYPE      portType, FMOD_PORT_INDEX portIndex,
                                                        FMOD::ChannelGroup* channelGroup, bool        passThru)
{
	return system->attachChannelGroupToPort(portType, portIndex, channelGroup, passThru);
}

FMOD_RESULT SoundSystemModule::detachChannelGroupFromPort(FMOD::ChannelGroup* channelGroup)
{
	return system->detachChannelGroupFromPort(channelGroup);
}

FMOD_RESULT SoundSystemModule::setReverbProperties(int instance, const FMOD_REVERB_PROPERTIES* prop)
{
	return system->setReverbProperties(instance, prop);
}

FMOD_RESULT SoundSystemModule::getReverbProperties(int instance, FMOD_REVERB_PROPERTIES* prop)
{
	return system->getReverbProperties(instance, prop);
}

void SoundSystemModule::Init()
{
	Module::Init();
}

void SoundSystemModule::Start()
{
	Module::Start();
}

void SoundSystemModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void SoundSystemModule::Update()
{
	Module::Update();
}

void SoundSystemModule::PreRender()
{
	Module::PreRender();
}

void SoundSystemModule::Render()
{
	Module::Render();
}

void SoundSystemModule::RenderGui()
{
	Module::RenderGui();
}

void SoundSystemModule::PostRender()
{
	Module::PostRender();
}

void SoundSystemModule::Release()
{
	Module::Release();
}

void SoundSystemModule::Finalize()
{
	Module::Finalize();
}
