#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "rhi.h"

#include <functional>


#include "Module.h"
#include "Modules/Module.h"
#include "ModuleManager.h"
#include "Modules/WindowModule.h"
#include "Modules/SoundSystemModule.h"
#include "Modules/WindowModule.h"

#include "ImGUIInterface.h"
#include "ImGUIAudio.h"

#include "GameObject/PreGameObject/LightGameObject.h"
#include "Scene/SceneManager.h"
#include "RHIVulkanModule.h"
#include "TCP/Errors.h"
#include <cstdarg>

#include "TCP/Client/TCPClientStart.h"
#include "TCP/Server/TCPServerStart.h"
#include "TCP/StatusMessage.h"
#include "TCP/Server/Server.h"

#include "Console.h"

class RHIModule;

/**
 * @brief Classe d�di�e � la gestion de l'interface utilisateur avec ImGui dans le contexte du module de rendu.
 */
class ImGuiModule final : public Module
{
public:

	//TimeModule* timeModule = nullptr;
	vk::Fence         immFence;
	vk::CommandBuffer immCommandBuffer;
	vk::CommandPool   immCommandPool;


	// ----------========== IMGUI SETTINGS ==========---------- //

	/**
	 * @brief Initialise le module ImGui, pr�parant les ressources n�cessaires pour son fonctionnement.
	 */
	void Init() override;

	/**
	 * @brief D�marre le module ImGui et pr�pare l'interface utilisateur pour l'interaction.
	 */
	void Start() override;

	/**
	 * @brief Ex�cute des mises � jour r�guli�res � intervalles fixes, typiquement pour la logique de jeu.
	 */
	void FixedUpdate() override;

	/**
	 * @brief Met � jour l'�tat du module ImGui avant le rendu.
	 */
	void Update() override;

	/**
	 * @brief Pr�pare le module ImGui pour le rendu, mais avant que le rendu ne soit effectu�.
	 */
	void PreRender() override;

	/**
	 * @brief Ex�cute le rendu de l'interface utilisateur ImGui.
	 */
	void Render() override;

	/**
	 * @brief Dessine l'interface graphique sp�cifique au module, comme les menus et les fen�tres.
	 */
	void RenderGui() override;

	/**
	 * @brief Op�rations � effectuer apr�s le rendu principal, souvent utilis�es pour les nettoyages.
	 */
	void PostRender() override;

	/**
	 * @brief Lib�re les ressources utilis�es par le module ImGui.
	 */
	void Release() override;

	/**
	 * @brief Nettoie l'�tat du module ImGui et ferme l'interface utilisateur.
	 */
	void Finalize() override;

	/**
	 * @brief Soumet une commande graphique imm�diate � ex�cuter.
	 * @param _function La fonction qui contient les commandes graphiques � ex�cuter.
	 */
	void ImmediateSubmit(std::function<void(vk::CommandBuffer _cmd)>&& _function) const;


	// ----------========== IMGUI SHOWN ==========---------- //

	/**
	 * @brief Construit les �l�ments de l'interface utilisateur � afficher, incluant les fen�tres "Hierarchy" et "Inspector".
	 */
	void GetGui();

	/**
	 * @brief Fixe la fen�tre de l'interface utilisateur � une position sp�cifique.
	 * @param _windowName Nom de la fen�tre � ancrer.
	 */
	void AnchorWindow(const std::string& _windowName);


	// ----------========== DRAW WINDOWS ==========---------- //

	/**
	 * @brief Dessine la hi�rarchie des objets de la sc�ne dans l'interface utilisateur.
	 */
	void DrawHierarchyWindow();

	/**
	 * @brief Dessine l'inspecteur des propri�t�s pour l'objet s�lectionn�.
	 */
	void DrawInspectorWindow();

	/**
	 * @brief Dessine la fen�tre de chat.
	 */
	void DrawTchatWindow();

	/**
	 * @brief Affiche et permet l'�dition des propri�t�s de transformation d'un objet.
	 * @param _transform Pointeur vers l'objet Transform � afficher et �diter.
	 */
	void DisplayTransform(Transform* _transform);

	void DrawModesWindow();

	/**
	 * @brief Dessine les param�tres de l'interface utilisateur du moteur de jeu.
	 */
	void DrawSettingsWindow();

	void DrawConsoleWindow();

	void DrawFilesExplorerWindow();

	void DrawProjectSaveWindow();



	// ----------========== POPUPS ==========---------- //

	/**
	 * @brief Affiche la popup de renommage pour l'entit� s�lectionn�e.
	 */
	void ShowRenamePopup();


	// ----------========== OBJECT OPERATIONS ==========---------- //

	/**
	 * @brief Renomme l'objet de jeu s�lectionn�.
	 * @param _gameObject Pointeur vers l'objet de jeu � renommer.
	 * @param _newName Nouveau nom � attribuer � l'objet de jeu.
	 */
	void RenameGameObject(GameObject* _gameObject, const std::string& _newName);

	/**
	 * @brief Supprime l'objet de jeu s�lectionn� de la sc�ne.
	 * @param _gameObject Pointeur vers l'objet de jeu � supprimer.
	 */
	void DeleteGameObject(GameObject* _gameObject);

	/**
	 * @brief Duplique l'objet de jeu situ� � l'index sp�cifi�.
	 * @param _gameObject Pointeur vers l'objet de jeu � � dupliquer.
	 */
	void DuplicateGameObject(GameObject* _gameObject);

	/**
	 * @brief Cr�e un nouveau GameObject du type sp�cifi� et l'ajoute � la sc�ne active.
	 * @param _type Type de GameObject � cr�er, comme Cube, Light ou Plane.
	 * @param _otherType
	 */
	void CreateSpecificGameObject(GameObjectType _type, int _otherType = 0);

	void AddLog(std::string _newText) { 
		Console& console = Console::getInstance();
		console.AddLog(_newText);
	}
	void ClearLog() {
		Console& console = Console::getInstance();
		console.ClearLogs();
	}

	bool GetFilterWarning() { return filterWarning; }
	bool GetFilterError() { return filterError; }
	bool GetFilterSimple() { return filterSimple; }

	void SetFilterWarning(bool _newState) { filterWarning = _newState; }
	void SetFilterError(bool _newState) { filterError = _newState; }
	void SetFilterSimple(bool _newState) { filterSimple = _newState; }

	std::string GetFileToLook() { return fileToLook; }
	void SetFileToLook(std::string _newFileToLook) { fileToLook = _newFileToLook; }

	std::string GetCurrentDir() { return currentDir; }
	void SetCurrentDir(std::string _newDir) { currentDir = _newDir; }

	std::string GetFileSearch() { return fileSearch; }
	void SetFileSearch(std::string _newFileSearch) { fileSearch = _newFileSearch; }

	bool GetFilterObj() { return filterObj; }
	bool GetFilterSupportedImages() { return filterSupportedImages; }
	bool GetFilterOther() { return filterOther; }
	bool GetFilterDirs() { return filterDirs; }


	void SetFilterObj(bool _newState) { filterObj = _newState; }
	void SetFilterSupportedImages(bool _newState) { filterSupportedImages = _newState; }
	void SetFilterOther(bool _newState) { filterOther = _newState; }
	void SetFilterDirs(bool _newState) { filterDirs = _newState; }

	bool GetShowPopupError() { return showPopupError; }
	void SetShowPopupError(bool _newState) { showPopupError = _newState; }

	std::string GetSceneToName() { return sceneToName; }
	void SetSceneToName(std::string _newString) { sceneToName = _newString; }

protected:
	vk::Device device; ///< P�riph�rique utilis� pour le rendu avec Vulkan.
	vk::Queue graphicsQueue; ///< File d'attente graphique pour la soumission des commandes Vulkan.

	TCPClientStart clientTCP;
	TCPServerStart serverTCP;
	bool isConnectedTCPClient = false;
	bool isConnectedTCPServer = false;

	WindowModule* windowModule = nullptr; ///< Module de gestion de la fenêtre de l'application.
	RHIModule* rhiModule = nullptr; ///< Module de gestion du rendu bas-niveau (RHI).
	SceneManager* sceneManager = nullptr; ///< Gestionnaire de sc�nes de l'application.

	GameObject* selectedGameObject = nullptr; ///< Pointeur vers l'objet de jeu actuellement s�lectionn�.

	bool isRenamePopupOpen = false; ///< Indique si la popup de renommage est ouverte ou non.
	int entityToRename = -1; ///< Index de l'entit� � renommer.
	char renameBuffer[256]; ///< Buffer pour le nouveau nom de l'entit� � renommer.
	int sceneToRename = -1; ///< Index de la sc�ne � renommer.
	char renameSceneBuffer[100]; ///< Buffer pour le nouveau nom de la sc�ne � renommer.

	bool openPositionEdit = false; ///< Indique si l'�dition de la position est ouverte ou non.
	bool openScaleEdit = false; ///< Indique si l'�dition de l'�chelle est ouverte ou non.
	glm::vec3 positionEdit; ///< Stocke la position actuelle pour l'�dition.
	float rotationEdit; ///< Stocke la rotation actuelle pour l'�dition.
	glm::vec3 scaleEdit; ///< Stocke l'�chelle actuelle pour l'�dition.

	std::vector<bool> isOpen; ///< Vecteur pour suivre l'�tat ouvert/ferm� des diff�rents panneaux de l'interface utilisateur.

	bool textureView = false;
	bool changeScaleLinked = false;
	bool filterSimple = true;
	bool filterError = true;
	bool filterWarning = true;

	bool filterSupportedImages = true;
	bool filterObj = true;
	bool filterOther = true;
	bool filterDirs = true;

	bool refreshFileExplorer = true;
	bool addTexButton = false;

	bool showPopupError = false;
	bool showPopupProject = true;

	std::vector<std::wstring> fileNames;

	std::vector<VkDescriptorSet>* ListDescriptorsImGui = new std::vector<VkDescriptorSet>();

	std::string fileSearch;


	std::string fileToLook;
	std::string currentDir = "./Textures";
	std::string sceneToName;

	std::vector<std::string>* logs = new std::vector<std::string>();

	char ipBuffer[64] = "";  // Buffer pour l'adresse IP
	char portBuffer[6] = ""; // Buffer pour le port, suffisant pour contenir des nombres jusqu'� 65535
	char messageBuffer[256] = ""; // Buffer pour le message � envoyer
	std::vector<std::string> messageLogs; // Liste des messages re�us

	ImGUIAudio* imGuiAudio; ///< Interface utilisateur pour les contr�les audio.
	SoundSystemModule* soundModule; ///< Module de gestion du syst�me audio.

	/**
	 * @brief Destructeur par d�faut.
	 */
	~ImGuiModule() = default;
};
