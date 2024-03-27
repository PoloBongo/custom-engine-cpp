#include "Managers/SoundSystemManager.h"
#include <fmod_errors.h>

SoundSystemManager::SoundSystemManager()
{
    if (FMOD::System_Create(&system) != FMOD_OK)
    {
        std::cout << "pas load" << std::endl;
        return;
    }

    int driverCount = 0;
    system->getNumDrivers(&driverCount);

    if (driverCount == 0)
    {
        std::cout << "error" << std::endl;
        return;
    }

    // Initialize our Instance with 36 Channels
    system->init(36, FMOD_LOOP_OFF, NULL);
}

void SoundSystemManager::createSound(SoundClass* pSound, const char* pathAudio)
{
    FMOD_RESULT result = system->createSound(pathAudio, FMOD_LOOP_OFF, 0, pSound);

    if (result != FMOD_OK) {
        std::cout << "le son n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
        return;
    }
}

void SoundSystemManager::playSound(SoundClass pSound, bool isPlay)
{
    if (!isPlay)
    {
        pSound->setMode(FMOD_LOOP_OFF);
    }
    else
    {
        pSound->setMode(FMOD_LOOP_NORMAL);
        pSound->setLoopCount(-1);
    }

    system->playSound(pSound, channelGroup, false, 0);
}

void SoundSystemManager::releaseSound(SoundClass pSound)
{
    pSound->release();
}

void SoundSystemManager::createChannelGroup(FMOD::ChannelGroup** channelGroup)
{
    system->createChannelGroup(NULL, channelGroup);
}

void SoundSystemManager::addSoundToGroup(SoundClass pSound, FMOD::ChannelGroup* channelGroup)
{
    system->playSound(pSound, channelGroup, false, nullptr);
}

// Implémentation des fonctions pour le routing vers les ports et la gestion de la réverbération
FMOD_RESULT SoundSystemManager::attachChannelGroupToPort(FMOD_PORT_TYPE portType, FMOD_PORT_INDEX portIndex, FMOD::ChannelGroup* channelGroup, bool passThru)
{
    return system->attachChannelGroupToPort(portType, portIndex, channelGroup, passThru);
}

FMOD_RESULT SoundSystemManager::detachChannelGroupFromPort(FMOD::ChannelGroup* channelGroup)
{
    return system->detachChannelGroupFromPort(channelGroup);
}

FMOD_RESULT SoundSystemManager::setReverbProperties(int instance, const FMOD_REVERB_PROPERTIES* prop)
{
    return system->setReverbProperties(instance, prop);
}

FMOD_RESULT SoundSystemManager::getReverbProperties(int instance, FMOD_REVERB_PROPERTIES* prop)
{
    return system->getReverbProperties(instance, prop);
}