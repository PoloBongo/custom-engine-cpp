#pragma once

#include <fmod.hpp> // Inclure la bibliothèque FMOD
#include <cstddef> // Pour inclure la définition de NULL
#include <iostream> // Pour les entrées/sorties standard

typedef FMOD::Sound* SoundClass; // Définition du type pour faciliter l'utilisation de la classe Sound de FMOD

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
    void createSound(SoundClass* pSound, const char* pathAudio);

    /**
     * @brief Joue un son spécifié
     * @param pSound Pointeur vers l'objet Sound à jouer
     * @param isPlay Booléen indiquant si le son doit être joué immédiatement (par défaut, false)
     */
    void playSound(SoundClass pSound, bool isPlay = false);

    /**
     * @brief Libère les ressources utilisées par un son spécifié
     * @param pSound Pointeur vers l'objet Sound à libérer
     */
    void releaseSound(SoundClass pSound);

    /**
     * @brief Crée un groupe de canaux pour le système audio
     * @param channelGroup Pointeur vers le groupe de canaux créé
     */
    void createChannelGroup(FMOD::ChannelGroup** channelGroup);

    /**
     * @brief Ajoute un son à un groupe de canaux spécifié
     * @param pSound Pointeur vers l'objet Sound à ajouter au groupe de canaux
     * @param channelGroup Pointeur vers le groupe de canaux auquel ajouter le son
     */
    void addSoundToGroup(SoundClass pSound, FMOD::ChannelGroup* channelGroup);

    /**
     * @brief Attache un groupe de canaux à un port spécifié pour le rendu audio.
     * @param portType Type de port auquel attacher le groupe de canaux (par ex. FMOD_PORT_TYPE_PS3_SPDIF).
     * @param portIndex Index du port auquel attacher le groupe de canaux.
     * @param channelGroup Pointeur vers le groupe de canaux à attacher.
     * @param passThru Indique si le signal doit être acheminé directement sans traitement (par défaut, false).
     * @return Résultat de l'opération d'attache du groupe de canaux au port.
     */
    FMOD_RESULT attachChannelGroupToPort(FMOD_PORT_TYPE portType, FMOD_PORT_INDEX portIndex, FMOD::ChannelGroup* channelGroup, bool passThru = false);

    /**
     * @brief Détache un groupe de canaux d'un port auquel il était précédemment attaché.
     * @param channelGroup Pointeur vers le groupe de canaux à détacher du port.
     * @return Résultat de l'opération de détachement du groupe de canaux du port.
     */
    FMOD_RESULT detachChannelGroupFromPort(FMOD::ChannelGroup* channelGroup);

    /**
     * @brief Définit les propriétés de réverbération pour une instance de réverbération spécifiée.
     * @param instance Instance de réverbération pour laquelle définir les propriétés.
     * @param prop Pointeur vers les propriétés de réverbération à définir.
     * @return Résultat de l'opération de définition des propriétés de réverbération.
     */
    FMOD_RESULT setReverbProperties(int instance, const FMOD_REVERB_PROPERTIES* prop);

    /**
     * @brief Obtient les propriétés de réverbération pour une instance de réverbération spécifiée.
     * @param instance Instance de réverbération pour laquelle obtenir les propriétés.
     * @param prop Pointeur vers les propriétés de réverbération à remplir avec les valeurs obtenues.
     * @return Résultat de l'opération de récupération des propriétés de réverbération.
     */
    FMOD_RESULT getReverbProperties(int instance, FMOD_REVERB_PROPERTIES* prop);

private:
    FMOD::System* system; // Pointeur vers l'objet système FMOD
    FMOD::ChannelGroup* channelGroup; // Pointeur vers le groupe de canaux utilisé
};
