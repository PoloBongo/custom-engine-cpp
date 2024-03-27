#include "SoundSystemClass.h"
#include <fmod_errors.h>

SoundSystemClass::SoundSystemClass()
{
    std::cout << "load instance soundSystemClass" << std::endl;
    if (FMOD::System_Create(&m_pSystem) != FMOD_OK)
    {
        std::cout << "pas load" << std::endl;
        return;
    }

    int driverCount = 0;
    m_pSystem->getNumDrivers(&driverCount);

    if (driverCount == 0)
    {
        std::cout << "error" << std::endl;
        return;
    }

    // Initialize our Instance with 36 Channels
    m_pSystem->init(36, FMOD_INIT_NORMAL, NULL);
}

void SoundSystemClass::createSound(SoundClass* pSound, const char* pFile)
{
    FMOD_RESULT result = m_pSystem->createSound(pFile, FMOD_LOOP_NORMAL, 0, pSound);
    if (result != FMOD_OK) {
        std::cout << "Erreur lors de la création du son : " << FMOD_ErrorString(result) << std::endl;
        return;
    }
}

void SoundSystemClass::playSound(SoundClass pSound, bool bLoop = false)
{
    std::cout << "enter function PlaySound()" << std::endl;

    if (!bLoop)
    {
        pSound->setMode(FMOD_LOOP_OFF);
    }
    else
    {
        pSound->setMode(FMOD_LOOP_NORMAL);
        pSound->setLoopCount(-1);
    }

    m_pSystem->playSound(pSound, channelGroup, false, 0);
}

void SoundSystemClass::releaseSound(SoundClass pSound)
{
    pSound->release();
}