#pragma once

#include <functional>
#include "Modules/Module.h"
#include "Modules/WindowModule.h"
#include "Scene/SceneManager.h"
#include "RHIVulkanModule.h"

class RHIModule;

/**
 * @brief Classe dédiée à la gestion de l'interface utilisateur avec ImGui dans le contexte du module de rendu.
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
	 * @brief Initialise le module ImGui, préparant les ressources nécessaires pour son fonctionnement.
	 */
	void Init() override;

	/**
	 * @brief Démarre le module ImGui et prépare l'interface utilisateur pour l'interaction.
	 */
	void Start() override;

	/**
	 * @brief Exécute des mises à jour régulières à intervalles fixes, typiquement pour la logique de jeu.
	 */
	void FixedUpdate() override;

	/**
	 * @brief Met à jour l'état du module ImGui avant le rendu.
	 */
	void Update() override;

	/**
	 * @brief Prépare le module ImGui pour le rendu, mais avant que le rendu ne soit effectué.
	 */
	void PreRender() override;

	/**
	 * @brief Exécute le rendu de l'interface utilisateur ImGui.
	 */
	void Render() override;

	/**
	 * @brief Dessine l'interface graphique spécifique au module, comme les menus et les fenêtres.
	 */
	void RenderGui() override;

	/**
	 * @brief Opérations à effectuer après le rendu principal, souvent utilisées pour les nettoyages.
	 */
	void PostRender() override;

	/**
	 * @brief Libère les ressources utilisées par le module ImGui.
	 */
	void Release() override;

	/**
	 * @brief Nettoie l'état du module ImGui et ferme l'interface utilisateur.
	 */
	void Finalize() override;

	/**
	 * @brief Soumet une commande graphique immédiate à exécuter.
	 * @param _function La fonction qui contient les commandes graphiques à exécuter.
	 */
	void ImmediateSubmit(std::function<void(vk::CommandBuffer _cmd)>&& _function) const;


	// ----------========== IMGUI SHOWN ==========---------- //

	/**
	 * @brief Construit les éléments de l'interface utilisateur à afficher, incluant les fenêtres "Hierarchy" et "Inspector".
	 */
	void GetGui();

	/**
	 * @brief Fixe la fenêtre de l'interface utilisateur à une position spécifique.
	 * @param _windowName Nom de la fenêtre à ancrer.
	 */
	void AnchorWindow(const std::string& _windowName);


	// ----------========== DRAW WINDOWS ==========---------- //

	/**
	 * @brief Dessine la hiérarchie des objets de la scène dans l'interface utilisateur.
	 */
	void DrawHierarchyWindow();

	/**
	 * @brief Dessine l'inspecteur des propriétés pour l'objet sélectionné.
	 */
	void DrawInspectorWindow();

	/**
	 * @brief Affiche et permet l'édition des propriétés de transformation d'un objet.
	 * @param _transform Pointeur vers l'objet Transform à afficher et éditer.
	 */
	void DisplayTransform(Transform* _transform);

	void DrawModesWindow();

	/**
	 * @brief Dessine les paramètres de l'interface utilisateur du moteur de jeu.
	 */
	void DrawSettingsWindow();


	// ----------========== POPUPS ==========---------- //

	/**
	 * @brief Affiche la popup de renommage pour l'entité sélectionnée.
	 */
	void ShowRenamePopup();


	// ----------========== OBJECT OPERATIONS ==========---------- //

	/**
	 * @brief Renomme l'objet de jeu sélectionné.
	 * @param _gameObject Pointeur vers l'objet de jeu à renommer.
	 * @param _newName Nouveau nom à attribuer à l'objet de jeu.
	 */
	void RenameGameObject(GameObject* _gameObject, const std::string& _newName);

	/**
	 * @brief Supprime l'objet de jeu sélectionné de la scène.
	 * @param _gameObject Pointeur vers l'objet de jeu à supprimer.
	 */
	void DeleteGameObject(GameObject* _gameObject);

	/**
	 * @brief Duplique l'objet de jeu situé à l'index spécifié.
	 * @param _gameObject Pointeur vers l'objet de jeu à à dupliquer.
	 */
	void DuplicateGameObject(GameObject* _gameObject);

	/**
	 * @brief Crée un nouveau GameObject du type spécifié et l'ajoute à la scène active.
	 * @param _type Type de GameObject à créer, comme Cube, Light ou Plane.
	 */
	void CreateSpecificGameObject(GameObjectType _type, int _otherType = 0);

protected:
	vk::Device device; ///< Périphérique utilisé pour le rendu avec Vulkan.
	vk::Queue graphicsQueue; ///< File d'attente graphique pour la soumission des commandes Vulkan.

	WindowModule* windowModule = nullptr; ///< Module de gestion de la fenêtre de l'application.
	RHIModule* rhiModule = nullptr; ///< Module de gestion du rendu bas-niveau (RHI).
	SceneManager* sceneManager = nullptr; ///< Gestionnaire de scènes de l'application.

	GameObject* selectedGameObject = nullptr; ///< Pointeur vers l'objet de jeu actuellement sélectionné.

	bool isRenamePopupOpen = false; ///< Indique si la popup de renommage est ouverte ou non.
	int entityToRename = -1; ///< Index de l'entité à renommer.
	char renameBuffer[256]; ///< Buffer pour le nouveau nom de l'entité à renommer.
	int sceneToRename = -1; ///< Index de la scène à renommer.
	char renameSceneBuffer[100]; ///< Buffer pour le nouveau nom de la scène à renommer.

	bool openPositionEdit = false; ///< Indique si l'édition de la position est ouverte ou non.
	bool openScaleEdit = false; ///< Indique si l'édition de l'échelle est ouverte ou non.
	glm::vec3 positionEdit; ///< Stocke la position actuelle pour l'édition.
	float rotationEdit; ///< Stocke la rotation actuelle pour l'édition.
	glm::vec3 scaleEdit; ///< Stocke l'échelle actuelle pour l'édition.

	std::vector<bool> isOpen; ///< Vecteur pour suivre l'état ouvert/fermé des différents panneaux de l'interface utilisateur.

	bool textureView = false;

	/**
	 * @brief Destructeur par défaut.
	 */
	~ImGuiModule() = default;
};
