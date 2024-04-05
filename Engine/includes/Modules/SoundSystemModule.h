#pragma once

#include <cstddef>
#include <fmod.hpp>
#include <iostream>

#include "Module.h"

/**
 * @brief Classe SoundClass : Gère le système Sound utilisant la bibliothèque FMOD
 */
using SoundClass = FMOD::Sound*;

/**
 * @brief Classe SoundGroup : Gère le système SoundGroup utilisant la bibliothèque FMOD
 */
using SoundGroup = FMOD::SoundGroup*;

/**
 * @brief Classe Channel : Gère les canaux audio utilisant la bibliothèque FMOD
 */
using Channel = FMOD::Channel*;

/**
 * @brief Classe SoundSystemModule : Gère le système audio utilisant la bibliothèque FMOD
 */
class SoundSystemModule final : public Module
{
	public:
		/**
		 * @brief Constructeur par défaut de la classe SoundSystemModule
		 */
		SoundSystemModule();

		/**
		 * @brief Crée un objet Sound à partir d'un fichier audio spécifié
		 * @param p_Sound Pointeur vers l'objet Sound créé
		 * @param _pathAudio Chemin du fichier audio à charger
		 */
		void CreateSound(SoundClass* p_Sound, const char* p_pathAudio);

		/**
		 * @brief Crée un objet SoundGroup à partir d'un fichier audio spécifié
		 * @param _pSoundGroup Pointeur vers l'objet SoundGroup créé
		 * @param _pathAudio Chemin du fichier audio à charger
		 */
		void CreateSoundGroup(SoundGroup* p_pSoundGroup, const char* p_pathAudio);

		/**
		 * @brief Obtient le groupe de sons principal
		 * @param _pSound Pointeur vers le groupe de sons principal
		 */
		void GetMasterSoundGroup(SoundGroup* p_pSound);

		/**
		 * @brief Joue un son spécifié
		 * @param _pSound Pointeur vers l'objet Sound à jouer
		 * @param _isPlay Booléen indiquant si le son doit être joué immédiatement
		 * @param _loopCount Nombre de fois que le son doit être joué en boucle
		 * @param _volume Volume du son à jouer
		 * @param p_channelPtr Pointeur vers le canal sur lequel le son est joué
		 */
		void PlaySound(SoundClass _pSound, bool _isPlay, int _loopCount, float _volume, Channel* p_channelPtr);

		/**
		 * @brief Libère les ressources utilisées par un son spécifié
		 * @param _pSound Pointeur vers l'objet Sound à libérer
		 */
		void ReleaseSound(SoundClass _pSound);

		/**
		 * @brief Crée un groupe de canaux pour le système audio
		 * @param pp_channelGroup Pointeur vers le groupe de canaux créé
		 */
		// ReSharper disable once CppInconsistentNaming
		void createChannelGroup(FMOD::ChannelGroup** pp_channelGroup);

		/**
		 * @brief Ajoute un son à un groupe de canaux spécifié
		 * @param _pSound Pointeur vers l'objet Sound à ajouter au groupe de canaux
		 * @param _channelGroup Pointeur vers le groupe de canaux auquel ajouter le son
		 */
		void AddSoundToGroup(SoundClass _pSound, FMOD::ChannelGroup* p_channelGroup);

		/**
		 * @brief Définit la distance minimum et maximum pour un son 3D
		 * @param _min Distance minimum du son
		 * @param _max Distance maximum du son
		 * @return La distance minimum et maximum définie
		 */
		float Set3DMinMaxDistance(const float _min, const float _max) { min = _min; max = _max; }

		/**
		 * @brief Obtient la distance minimum et maximum pour un son 3D
		 * @return La distance minimum et maximum du son
		 */
		float Get3DMinMaxDistance() { return min, max; }

		/**
		 * @brief Obtient le nombre de boucles d'un son spécifié
		 * @param _pSound Pointeur vers l'objet Sound
		 * @param p_loopCount Pointeur vers la variable pour stocker le nombre de boucles
		 * @return Le nombre de boucles du son spécifié
		 */
		int GetLoopCount(SoundClass _pSound, int* p_loopCount);

		/**
		 * @brief Attache un groupe de canaux à un port spécifié pour le rendu audio
		 * @param _portType Type de port auquel attacher le groupe de canaux
		 * @param _portIndex Index du port auquel attacher le groupe de canaux
		 * @param p_channelGroup Pointeur vers le groupe de canaux à attacher
		 * @param _passThru Indique si le signal doit être acheminé directement sans traitement (par défaut, false)
		 * @return Résultat de l'opération d'attache du groupe de canaux au port
		 */
		FMOD_RESULT AttachChannelGroupToPort(FMOD_PORT_TYPE      _portType, FMOD_PORT_INDEX _portIndex,
		                                     FMOD::ChannelGroup* p_channelGroup, bool        _passThru = false);

		/**
		 * @brief Détache un groupe de canaux d'un port auquel il était précédemment attaché
		 * @param p_channelGroup Pointeur vers le groupe de canaux à détacher du port
		 * @return Résultat de l'opération de détachement du groupe de canaux du port
		 */
		FMOD_RESULT DetachChannelGroupFromPort(FMOD::ChannelGroup* p_channelGroup);

		/**
		 * @brief Définit les propriétés de réverbération pour une instance de réverbération spécifiée
		 * @param _instance Instance de réverbération pour laquelle définir les propriétés
		 * @param p_prop Pointeur vers les propriétés de réverbération à définir
		 * @return Résultat de l'opération de définition des propriétés de réverbération
		 */
		FMOD_RESULT SetReverbProperties(int _instance, const FMOD_REVERB_PROPERTIES* p_prop);

		/**
		 * @brief Obtient les propriétés de réverbération pour une instance de réverbération spécifiée
		 * @param _instance Instance de réverbération pour laquelle obtenir les propriétés
		 * @param p_prop Pointeur vers les propriétés de réverbération à remplir avec les valeurs obtenues
		 * @return Résultat de l'opération de récupération des propriétés de réverbération
		 */
		FMOD_RESULT GetReverbProperties(int _instance, FMOD_REVERB_PROPERTIES* p_prop);


		/**
		    * @brief Initialise le module.
		    */
		void Init() override;

		/**
		 * @brief Démarre le module.
		 */
		void Start() override;

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		void FixedUpdate() override;

		/**
		 * @brief Met à jour le module.
		 */
		void Update() override;

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		void PreRender() override;

		/**
		 * @brief Rendu du module.
		 */
		void Render() override;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui() override;

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender() override;

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		void Release() override;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() override;

	private:
		FMOD::System*       system;       // Pointeur vers l'objet système FMOD
		FMOD::ChannelGroup* channelGroup; // Pointeur vers le groupe de canaux utilisé

		// Variables pour la distance minimum et maximum d'un son 3D
		float min;
		float max;
};
