#include "Modules/SoundSystemModule.h"
#include <fmod_errors.h>
#include <string>

SoundSystemModule::SoundSystemModule() : isInitialized(false) {
	FMOD_RESULT result = System_Create(&system);
	if (result != FMOD_OK) {
		std::cerr << "Erreur lors de la création du système FMOD: " << FMOD_ErrorString(result) << std::endl;
		return;  // Important pour éviter d'exécuter les étapes suivantes si le système n'est pas créé
	}

	int driverCount = 0;
	system->getNumDrivers(&driverCount);
	if (driverCount == 0) {
		std::cerr << "FMOD n'a pas trouvé de sortie de son." << std::endl;
		return;
	}

	result = system->init(36, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK) {
		std::cerr << "Erreur lors de l'initialisation de FMOD: " << FMOD_ErrorString(result) << std::endl;
		return;
	}

	std::cout << "SoundSystemModule initialisé avec succès." << std::endl;
}

// Permet la création du son avec son chemin d'accès //
void SoundSystemModule::CreateSound(SoundClass* p_Sound, const char* p_pathAudio) const
{
	const FMOD_RESULT result = system->createSound(p_pathAudio, FMOD_LOOP_OFF, nullptr, p_Sound);

	if (result != FMOD_OK) std::cout << "le son n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet la création d'un groupe de son. //
void SoundSystemModule::CreateSoundGroup(SoundGroup* p_pSoundGroup, const char* p_pathAudio) const
{
	if (const FMOD_RESULT result = system->createSoundGroup(p_pathAudio, p_pSoundGroup);
		result != FMOD_OK) std::cout << "le groupe n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet la récupération d'un groupe de son. //
void SoundSystemModule::GetMasterSoundGroup(SoundGroup* p_pSound) const
{
	if (const FMOD_RESULT result = system->getMasterSoundGroup(p_pSound);
		result != FMOD_OK) std::cout << "le groupe n'a pas charge : " << FMOD_ErrorString(result) << std::endl;
}

// Permet de joué un son spécifique avec ces propres paramètres. //
void SoundSystemModule::PlaySound(const SoundClass _pSound, const bool _isPlay, const int _loopCount,
                                  const float      _volume, Channel*   p_channelPtr) const
{
	FMOD::Channel* channel = nullptr;

	if (!_isPlay)
	{
		_pSound->setMode(FMOD_LOOP_OFF);
	}
	else
	{
		_pSound->setMode(FMOD_LOOP_NORMAL);
		_pSound->setLoopCount(_loopCount);
	}

	system->playSound(_pSound, channelGroup, false, &channel);

	channel->setVolume(_volume);

	// Affecter le canal créé au pointeur de pointeur passé en paramètre
	*p_channelPtr = channel;
}

void SoundSystemModule::StopSound() {
	if (mainChannel) {
		mainChannel->stop();
		mainChannel = nullptr;	// Réinitialisez le canal principal après l'arrêt
		isPlaying = false;
	}
}

// Permet de libérer la mémoire et de couper le son. //
void SoundSystemModule::ReleaseSound(const SoundClass _pSound)
{
	_pSound->release();
}

// Permet d'obtenir le nombre total de tour sur le son //
int SoundSystemModule::GetLoopCount(const SoundClass _pSound, int* p_loopCount)
{
	return _pSound->getLoopCount(p_loopCount);
}

// Permet la création d'un channel de groupe. //
void SoundSystemModule::CreateChannelGroup(FMOD::ChannelGroup** channelGroup) const
{
	system->createChannelGroup(nullptr, channelGroup);
}

// Permet d'ajouter un son à un channel de groupe pour pouvoir modifier les paramètres du son du tout les sons du channel. //
void SoundSystemModule::AddSoundToGroup(const SoundClass _pSound, FMOD::ChannelGroup* p_channelGroup) const
{
	system->playSound(_pSound, p_channelGroup, false, nullptr);
}

float SoundSystemModule::GetMasterVolume() const {
	float volume;
	if (mainChannel) {
		mainChannel->getVolume(&volume);
	}
	else {
		volume = 0.0f; // Aucun canal actif, retourne 0 ou un état de volume inactif
	}
	return volume;
}

void SoundSystemModule::SetMasterVolume(float _volume) {
	if (mainChannel) {  
		mainChannel->setVolume(_volume);
	}
}

bool SoundSystemModule::IsPlaying() const {
	return isPlaying;
}

void SoundSystemModule::TogglePlayPause() {
	if (mainChannel) {
		bool currentlyPaused;
		mainChannel->getPaused(&currentlyPaused);
		mainChannel->setPaused(!currentlyPaused);
		isPlaying = !currentlyPaused;
	}
}

std::string SoundSystemModule::GetCurrentTrackName() const {
	return currentTrackName;
}

void SoundSystemModule::loadAndPlaySound(const char* filePath) {
	if (!isPlaying || currentTrackName != filePath) {  // Vérifier si le son n'est pas déjà en cours de lecture ou si un nouveau fichier est choisi
		if (mainChannel) {
			mainChannel->stop();  // Arrêtez le son actuel s'il y en a un
		}

		FMOD::Sound* newSound;
		FMOD_RESULT result = system->createSound(filePath, FMOD_DEFAULT, nullptr, &newSound);
		if (result == FMOD_OK) {
			system->playSound(newSound, nullptr, false, &mainChannel);
			currentSound = newSound;
			currentTrackName = filePath;
			isPlaying = true;
		}
		else {
			std::cerr << "Failed to load sound: " << FMOD_ErrorString(result) << std::endl;
		}
	}
}

// Implémentation des fonctions pour le routing vers les ports et la gestion de la réverbération //
FMOD_RESULT SoundSystemModule::AttachChannelGroupToPort(const FMOD_PORT_TYPE _portType,
                                                        const FMOD_PORT_INDEX _portIndex,
                                                        FMOD::ChannelGroup* p_channelGroup, const bool _passThru) const
{
	return system->attachChannelGroupToPort(_portType, _portIndex, p_channelGroup, _passThru);
}

FMOD_RESULT SoundSystemModule::DetachChannelGroupFromPort(FMOD::ChannelGroup* p_channelGroup) const
{
	return system->detachChannelGroupFromPort(p_channelGroup);
}

FMOD_RESULT SoundSystemModule::SetReverbProperties(const int _instance, const FMOD_REVERB_PROPERTIES* p_prop) const
{
	return system->setReverbProperties(_instance, p_prop);
}

FMOD_RESULT SoundSystemModule::GetReverbProperties(const int _instance, FMOD_REVERB_PROPERTIES* p_prop) const
{
	return system->getReverbProperties(_instance, p_prop);
}

void SoundSystemModule::Init()
{
	if (FMOD::System_Create(&system) != FMOD_OK) {
		std::cerr << "Erreur lors de la création du système FMOD." << std::endl;
		isInitialized = false;
		return;
	}

	int driverCount = 0;
	system->getNumDrivers(&driverCount);
	if (driverCount == 0) {
		std::cerr << "FMOD n'a pas trouvé de sortie de son." << std::endl;
		isInitialized = false;
		return;
	}

	if (system->init(36, FMOD_INIT_NORMAL, nullptr) != FMOD_OK) {
		std::cerr << "Échec de l'initialisation de FMOD." << std::endl;
		isInitialized = false;
		return;
	}

	isInitialized = true;

	Module::Init();
}

void SoundSystemModule::Start()
{
	if (!isInitialized) {
		std::cerr << "Tentative de démarrage d'un module non initialisé." << std::endl;
		return;
	}
	Module::Start();
	std::cout << "SoundSystemModule::Start appelé." << std::endl;
}

void SoundSystemModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void SoundSystemModule::Update()
{
	if (system) {
		system->update();
	}
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
