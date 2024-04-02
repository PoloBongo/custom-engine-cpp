#pragma once

#include <fmod.hpp>
#include <cstddef>
#include <iostream>

/**
 * @brief Classe SoundClass : Gère le système Sound utilisant la bibliothèque FMOD
 */
typedef FMOD::Sound* SoundClass;

/**
 * @brief Classe SoundGroup : Gère le système SoundGroup utilisant la bibliothèque FMOD
 */
typedef FMOD::SoundGroup* SoundGroup;

/**
 * @brief Classe Channel : Gère les canaux audio utilisant la bibliothèque FMOD
 */
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
     * @param _pSound Pointeur vers l'objet Sound créé
     * @param _pathAudio Chemin du fichier audio à charger
     */
    void createSound(SoundClass* _pSound, const char* _pathAudio);

    /**
     * @brief Crée un objet SoundGroup à partir d'un fichier audio spécifié
     * @param _pSoundGroup Pointeur vers l'objet SoundGroup créé
     * @param _pathAudio Chemin du fichier audio à charger
     */
    void createSoundGroup(SoundGroup* _pSoundGroup, const char* _pathAudio);

    /**
     * @brief Obtient le groupe de sons principal
     * @param _pSound Pointeur vers le groupe de sons principal
     */
    void getMasterSoundGroup(SoundGroup* _pSound);

    /**
     * @brief Joue un son spécifié
     * @param _pSound Pointeur vers l'objet Sound à jouer
     * @param _isPlay Booléen indiquant si le son doit être joué immédiatement
     * @param _loopCount Nombre de fois que le son doit être joué en boucle
     * @param _volume Volume du son à jouer
     * @param _channelPtr Pointeur vers le canal sur lequel le son est joué
     */
    void playSound(SoundClass _pSound, bool _isPlay, int _loopCount, float _volume, Channel* _channelPtr);

    /**
     * @brief Libère les ressources utilisées par un son spécifié
     * @param _pSound Pointeur vers l'objet Sound à libérer
     */
    void releaseSound(SoundClass _pSound);

    /**
     * @brief Crée un groupe de canaux pour le système audio
     * @param _channelGroup Pointeur vers le groupe de canaux créé
     */
    void createChannelGroup(FMOD::ChannelGroup** _channelGroup);

    /**
     * @brief Ajoute un son à un groupe de canaux spécifié
     * @param _pSound Pointeur vers l'objet Sound à ajouter au groupe de canaux
     * @param _channelGroup Pointeur vers le groupe de canaux auquel ajouter le son
     */
    void addSoundToGroup(SoundClass _pSound, FMOD::ChannelGroup* _channelGroup);

    /**
     * @brief Définit la distance minimum et maximum pour un son 3D
     * @param _min Distance minimum du son
     * @param _max Distance maximum du son
     * @return La distance minimum et maximum définie
     */
    float set3DMinMaxDistance(float _min, float _max) { return min, max; }

    /**
     * @brief Obtient la distance minimum et maximum pour un son 3D
     * @return La distance minimum et maximum du son
     */
    float get3DMinMaxDistance() { return min, max; }

    /**
     * @brief Obtient le nombre de boucles d'un son spécifié
     * @param _pSound Pointeur vers l'objet Sound
     * @param _loopcount Pointeur vers la variable pour stocker le nombre de boucles
     * @return Le nombre de boucles du son spécifié
     */
    int getLoopCount(SoundClass _pSound, int* _loopcount);

    /**
     * @brief Attache un groupe de canaux à un port spécifié pour le rendu audio
     * @param _portType Type de port auquel attacher le groupe de canaux
     * @param _portIndex Index du port auquel attacher le groupe de canaux
     * @param _channelGroup Pointeur vers le groupe de canaux à attacher
     * @param _passThru Indique si le signal doit être acheminé directement sans traitement (par défaut, false)
     * @return Résultat de l'opération d'attache du groupe de canaux au port
     */
    FMOD_RESULT attachChannelGroupToPort(FMOD_PORT_TYPE _portType, FMOD_PORT_INDEX _portIndex, FMOD::ChannelGroup* _channelGroup, bool _passThru = false);

    /**
     * @brief Détache un groupe de canaux d'un port auquel il était précédemment attaché
     * @param _channelGroup Pointeur vers le groupe de canaux à détacher du port
     * @return Résultat de l'opération de détachement du groupe de canaux du port
     */
    FMOD_RESULT detachChannelGroupFromPort(FMOD::ChannelGroup* _channelGroup);

    /**
     * @brief Définit les propriétés de réverbération pour une instance de réverbération spécifiée
     * @param _instance Instance de réverbération pour laquelle définir les propriétés
     * @param _prop Pointeur vers les propriétés de réverbération à définir
     * @return Résultat de l'opération de définition des propriétés de réverbération
     */
    FMOD_RESULT setReverbProperties(int _instance, const FMOD_REVERB_PROPERTIES* _prop);

    /**
     * @brief Obtient les propriétés de réverbération pour une instance de réverbération spécifiée
     * @param _instance Instance de réverbération pour laquelle obtenir les propriétés
     * @param _prop Pointeur vers les propriétés de réverbération à remplir avec les valeurs obtenues
     * @return Résultat de l'opération de récupération des propriétés de réverbération
     */
    FMOD_RESULT getReverbProperties(int _instance, FMOD_REVERB_PROPERTIES* _prop);

private:
    FMOD::System* system; // Pointeur vers l'objet système FMOD
    FMOD::ChannelGroup* channelGroup; // Pointeur vers le groupe de canaux utilisé

    // Variables pour la distance minimum et maximum d'un son 3D
    float min;
    float max;
};