#pragma once

#include <fmod.hpp>
#include <cstddef>
#include <iostream>

/**
 * @brief Classe SoundClass : Gère le système Sound utilisant la bibliothèque FMOD
 */
typedef FMOD::Sound* SoundClass;
/**
 * @brief Classe SoundClass : Gère le système SoundGroup utilisant la bibliothèque FMOD
 */
typedef FMOD::SoundGroup* SoundGroup;

typedef FMOD::Channel* Channel;

/**
 * @brief Classe SoundSystemManager : Gère le système audio utilisant la bibliothèque FMOD
 */
class SoundSystemManager
{
public:
    /**
     * @brief Constructeur par défaut de la classe SoundSystemManager
     */
    SoundSystemManager();

    /**
     * @brief Crée un objet Sound à partir d'un fichier audio spécifié
     * @param pSound Pointeur vers l'objet Sound créé
     * @param pathAudio Chemin du fichier audio à charger
     */
    void createSound(SoundClass* _pSound, const char* _pathAudio);

    void createSoundGroup(SoundGroup* _pSoundGroup, const char* _pathAudio);

    void getMasterSoundGroup(SoundGroup* pSound);

    /**
     * @brief Joue un son spécifié
     * @param pSound Pointeur vers l'objet Sound à jouer
     * @param isPlay Booléen indiquant si le son doit être joué immédiatement
     * @param loopCount Int indiquant combien de fois l'audio doit ce jouer
     */
    void playSound(SoundClass _pSound, bool _isPlay, int loopCount, float volume, Channel* channelPtr);

    /**
     * @brief Libère les ressources utilisées par un son spécifié
     * @param pSound Pointeur vers l'objet Sound à libérer
     */
    void releaseSound(SoundClass _pSound);

    /**
     * @brief Crée un groupe de canaux pour le système audio
     * @param channelGroup Pointeur vers le groupe de canaux créé
     */
    void createChannelGroup(FMOD::ChannelGroup** _channelGroup);

    /**
     * @brief Ajoute un son à un groupe de canaux spécifié
     * @param pSound Pointeur vers l'objet Sound à ajouter au groupe de canaux
     * @param channelGroup Pointeur vers le groupe de canaux auquel ajouter le son
     */
    void addSoundToGroup(SoundClass _pSound, FMOD::ChannelGroup* _channelGroup);

    /**
    * @brief Set une distance minimum et maximum pour un son 3D
    * @param _min float pour la variable minimum
    * @param _max float pour la variable maximum
    */
    float set3DMinMaxDistance(float _min, float _max) { return min, max; }

    /**
    * @brief Get une distance minimum et maximum pour un son 3D
    * @return Résultat de la variable minimum et maximum
    */
    float get3DMinMaxDistance() { return min, max; }

    int getLoopCount(SoundClass _pSound, int* loopcount);

    /**
     * @brief Attache un groupe de canaux à un port spécifié pour le rendu audio.
     * @param portType Type de port auquel attacher le groupe de canaux (par ex. FMOD_PORT_TYPE_PS3_SPDIF).
     * @param portIndex Index du port auquel attacher le groupe de canaux.
     * @param channelGroup Pointeur vers le groupe de canaux à attacher.
     * @param passThru Indique si le signal doit être acheminé directement sans traitement (par défaut, false).
     * @return Résultat de l'opération d'attache du groupe de canaux au port.
     */
    FMOD_RESULT attachChannelGroupToPort(FMOD_PORT_TYPE _portType, FMOD_PORT_INDEX _portIndex, FMOD::ChannelGroup* _channelGroup, bool _passThru = false);

    /**
     * @brief Détache un groupe de canaux d'un port auquel il était précédemment attaché.
     * @param channelGroup Pointeur vers le groupe de canaux à détacher du port.
     * @return Résultat de l'opération de détachement du groupe de canaux du port.
     */
    FMOD_RESULT detachChannelGroupFromPort(FMOD::ChannelGroup* _channelGroup);

    /**
     * @brief Définit les propriétés de réverbération pour une instance de réverbération spécifiée.
     * @param instance Instance de réverbération pour laquelle définir les propriétés.
     * @param prop Pointeur vers les propriétés de réverbération à définir.
     * @return Résultat de l'opération de définition des propriétés de réverbération.
     */
    FMOD_RESULT setReverbProperties(int _instance, const FMOD_REVERB_PROPERTIES* _prop);

    /**
     * @brief Obtient les propriétés de réverbération pour une instance de réverbération spécifiée.
     * @param instance Instance de réverbération pour laquelle obtenir les propriétés.
     * @param prop Pointeur vers les propriétés de réverbération à remplir avec les valeurs obtenues.
     * @return Résultat de l'opération de récupération des propriétés de réverbération.
     */
    FMOD_RESULT getReverbProperties(int _instance, FMOD_REVERB_PROPERTIES* _prop);

private:
    FMOD::System* system; // Pointeur vers l'objet système FMOD
    FMOD::ChannelGroup* channelGroup; // Pointeur vers le groupe de canaux utilisé

    // float variable
    float min;
    float max;
};
