#pragma once
#include <fmod.hpp>
#include <fmod.h>
#include <cstddef>

#include <iostream>

typedef FMOD::Sound* SoundClass;

class SoundSystemClass
{
public:
    SoundSystemClass();

    void createSound(SoundClass* pSound, const char* pFile);

    void playSound(SoundClass pSound, bool bLoop);

    void releaseSound(SoundClass pSound);
private:
    FMOD::System* m_pSystem;
    FMOD::ChannelGroup* channelGroup;
};