#pragma once

#include <fmod.hpp>
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
		 * @param p_sound Pointeur vers l'objet Sound créé
		 * @param p_pathAudio Chemin du fichier audio à charger
		 */
		void CreateSound(SoundClass* p_sound, const char* p_pathAudio) const;

		/**
		 * @brief Crée un objet SoundGroup à partir d'un fichier audio spécifié
		 * @param p_soundGroup Pointeur vers l'objet SoundGroup créé
		 * @param p_pathAudio Chemin du fichier audio à charger
		 */
		void CreateSoundGroup(SoundGroup* p_soundGroup, const char* p_pathAudio) const;

		/**
		 * @brief Obtient le groupe de sons principal
		 * @param p_Sound Pointeur vers le groupe de sons principal
		 */
		void GetMasterSoundGroup(SoundGroup* p_Sound) const;

		/**
		 * @brief Joue un son spécifié
		 * @param p_sound Pointeur vers l'objet Sound à jouer
		 * @param _isPlay Booléen indiquant si le son doit être joué immédiatement
		 * @param _loopCount Nombre de fois que le son doit être joué en boucle
		 * @param _volume Volume du son à jouer
		 * @param p_channelPtr Pointeur vers le canal sur lequel le son est joué
		 */
		void PlaySound(SoundClass p_sound, bool _isPlay, int _loopCount, float _volume, Channel* p_channelPtr) const;

		/**
		 * @brief Libère les ressources utilisées par un son spécifié
		 * @param p_sound Pointeur vers l'objet Sound à libérer
		 */
		void ReleaseSound(SoundClass p_sound);

		/**
		 * @brief Crée un groupe de canaux pour le système audio
		 * @param pp_channelGroup Pointeur vers le groupe de canaux créé
		 */
		// ReSharper disable once CppInconsistentNaming
		void CreateChannelGroup(FMOD::ChannelGroup** pp_channelGroup) const;

		/**
		 * @brief Ajoute un son à un groupe de canaux spécifié
		 * @param _pSound Pointeur vers l'objet Sound à ajouter au groupe de canaux
		 * @param p_channelGroup Pointeur vers le groupe de canaux auquel ajouter le son
		 */
		void AddSoundToGroup(SoundClass _pSound, FMOD::ChannelGroup* p_channelGroup) const;

		/**
		 * @brief Obtient le nombre de boucles d'un son spécifié
		 * @param _pSound Pointeur vers l'objet Sound
		 * @param p_loopCount Pointeur vers la variable pour stocker le nombre de boucles
		 * @return Le nombre de boucles du son spécifié
		 */
		static int GetLoopCount(SoundClass _pSound, int* p_loopCount);

		/**
		 * @brief Attache un groupe de canaux à un port spécifié pour le rendu audio
		 * @param _portType Type de port auquel attacher le groupe de canaux
		 * @param _portIndex Index du port auquel attacher le groupe de canaux
		 * @param p_channelGroup Pointeur vers le groupe de canaux à attacher
		 * @param _passThru Indique si le signal doit être acheminé directement sans traitement (par défaut, false)
		 * @return Résultat de l'opération d'attache du groupe de canaux au port
		 */
		FMOD_RESULT AttachChannelGroupToPort(FMOD_PORT_TYPE      _portType, FMOD_PORT_INDEX _portIndex,
		                                     FMOD::ChannelGroup* p_channelGroup, bool       _passThru = false) const;

		/**
		 * @brief Détache un groupe de canaux d'un port auquel il était précédemment attaché
		 * @param p_channelGroup Pointeur vers le groupe de canaux à détacher du port
		 * @return Résultat de l'opération de détachement du groupe de canaux du port
		 */
		FMOD_RESULT DetachChannelGroupFromPort(FMOD::ChannelGroup* p_channelGroup) const;

		/**
		 * @brief Définit les propriétés de réverbération pour une instance de réverbération spécifiée
		 * @param _instance Instance de réverbération pour laquelle définir les propriétés
		 * @param p_prop Pointeur vers les propriétés de réverbération à définir
		 * @return Résultat de l'opération de définition des propriétés de réverbération
		 */
		FMOD_RESULT SetReverbProperties(int _instance, const FMOD_REVERB_PROPERTIES* p_prop) const;

		/**
		 * @brief Obtient les propriétés de réverbération pour une instance de réverbération spécifiée
		 * @param _instance Instance de réverbération pour laquelle obtenir les propriétés
		 * @param p_prop Pointeur vers les propriétés de réverbération à remplir avec les valeurs obtenues
		 * @return Résultat de l'opération de récupération des propriétés de réverbération
		 */
		FMOD_RESULT GetReverbProperties(int _instance, FMOD_REVERB_PROPERTIES* p_prop) const;

#pragma region Event

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

#pragma endregion

	private:
		/**< Pointeur vers l'objet système FMOD. */
		FMOD::System* system;

		/**< Pointeur vers le groupe de canaux utilisé. */
		FMOD::ChannelGroup* channelGroup;

		/**< Distance minimale d'un son 3D. */
		float min;

		/**< Distance maximale d'un son 3D. */
		float max;
};
