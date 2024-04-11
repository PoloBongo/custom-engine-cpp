#pragma once
#include "Module.h"

#include "lve_descriptors.h"
#include "lve_window.h"
//std
#include <string>

#include "GameObject/GameObject.h"


namespace Inputs
{
	enum class KeyAction;
	enum class MouseButton;
	enum class KeyCode;
}

class SceneManager;
/**
 * @brief Classe WindowModule.
 *
 * Cette classe représente le gestionnaire de fenêtres dans le système.
 * Elle hérite de la classe Module, ce qui lui permet d'être intégrée dans le système de modules.
 * Le WindowModule est responsable de la gestion et de la manipulation des fenêtres de l'application.
 */

enum GlfwCursorType
{
	ARROW,
	/**< Curseur flèche. */
	IBEAM,
	/**< Curseur I-beam (curseur de texte). */
	CROSSHAIR,
	/**< Curseur croix. */
	POINTING_HAND,
	/**< Curseur main pointant. */
	RESIZE_EW,
	/**< Curseur redimensionner horizontal. */
	RESIZE_NS,
	/**< Curseur redimensionner vertical. */
	RESIZE_NWSE,
	/**< Curseur redimensionner diagonale NW-SE. */
	RESIZE_NESW,
	/**< Curseur redimensionner diagonale NE-SW. */
	RESIZE_ALL,
	/**< Curseur redimensionner dans toutes les directions. */
	NOT_ALLOWED /**< Curseur non autorisé. */
};

/**
 * @brief Enumération des modes de curseurs GLFW.
 *
 * Cette énumération représente les différents modes de curseurs GLFW.
 * Ces modes définissent le comportement du curseur sur l'écran.
 */
enum GlfwCursorMode
{
	DISABLED,
	/**< Curseur désactivé. */
	HIDDEN,
	/**< Curseur caché. */
	NORMAL,
	/**< Curseur normal (visible). */
	CAPTURED /**< Curseur capturé (exclusif à la fenêtre). */
};

enum class WindowMode
{
	WINDOWED,
	WINDOWED_FULLSCREEN,
	// (aka "Borderless windowed")
	FULLSCREEN,

	_NONE
};

static const char* WindowModeStrings[] =
{
	"Windowed",
	"Windowed Fullscreen",
	"Fullscreen",

	"NONE"
};


class WindowModule final : public Module
{
public:
	/**
			 * @brief Enumération des types de curseurs GLFW.
			 *
			 * Cette énumération représente les différents types de curseurs GLFW disponibles.
			 * Ces types peuvent être utilisés pour spécifier le style du curseur à afficher.
			 */


	static constexpr int WIDTH = 800; ///< Largeur de la fenêtre par défaut.
	static constexpr int HEIGHT = 600; ///< Hauteur de la fenêtre par défaut.


	/**
	 * @brief Constructeur de la classe WindowModule.
	 *
	 * Initialise une instance de WindowModule avec les dimensions spécifiées et le nom de la fenêtre.
	 *
	 * @param _width La largeur de la fenêtre.
	 * @param _height La hauteur de la fenêtre.
	 * @param _name Le nom de la fenêtre.
	 * @return Rien.
	 */
	WindowModule();

	/**
	 * @brief Destructeur de la classe WindowModule.
	 *
	 * Détruit la fenêtre GLFW.
	 */
	~WindowModule();

	/**
	 * @brief Constructeur de copie supprimé.
	 *
	 * La copie d'objets de type WindowModule est explicitement interdite pour éviter les problèmes de gestion
	 * des ressources associées à la fenêtre.
	 */
	WindowModule(const WindowModule&) = delete;

	/**
	 * @brief Opérateur d'affectation par copie supprimé.
	 *
	 * L'affectation par copie d'objets de type WindowModule est explicitement interdite pour éviter les problèmes
	 * de gestion des ressources associées à la fenêtre.
	 *
	 * @return Une référence vers l'objet WindowModule après affectation.
	 */
	WindowModule& operator=(const WindowModule&) = delete;

	/**
	 * @brief Vérifie si la fenêtre doit être fermée.
	 *
	 * Cette fonction renvoie true si la fenêtre doit être fermée, false sinon.
	 *
	 * @return true si la fenêtre doit être fermée, false sinon.
	 */
	bool ShouldClose() const { return glfwWindowShouldClose(window); }

	/**
	 * @brief Obtient les dimensions de la fenêtre.
	 *
	 * Cette fonction retourne les dimensions de la fenêtre sous forme d'une structure vk::Extent2D.
	 *
	 * @return Les dimensions de la fenêtre sous forme d'une structure vk::Extent2D.
	 */
	vk::Extent2D GetExtent() const { return {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)}; }

	/**
	 * @brief Vérifie si la fenêtre a été redimensionnée.
	 *
	 * Cette fonction vérifie si la fenêtre a été redimensionnée depuis la dernière vérification.
	 *
	 * @return true si la fenêtre a été redimensionnée, sinon false.
	 */

	bool WasWindowResized() const { return bFrameBufferResize; }

	/**
	 * @brief Réinitialise le drapeau de redimensionnement de la fenêtre.
	 *
	 * Cette fonction réinitialise le drapeau indiquant que la fenêtre a été redimensionnée.
	 */
	void ResetWindowResizedFlag() { bFrameBufferResize = false; }

	/**
	 * @brief Crée une surface Vulkan associée à une fenêtre.
	 *
	 * Cette fonction crée une surface Vulkan associée à une fenêtre, permettant à Vulkan de dessiner dans cette fenêtre.
	 *
	 * @param _instance L'instance Vulkan utilisée pour créer la surface.
	 * @param _surface Un pointeur vers l'objet de surface Vulkan à créer. Ce pointeur sera mis à jour pour contenir la surface créée.
	 * @throws Une exception en cas d'échec lors de la création de la surface.
	 */
	void CreateWindowSurface(vk::Instance _instance, vk::SurfaceKHR* _surface) const;

	/**
	 * @brief Obtient un pointeur vers la fenêtre GLFW.
	 *
	 * Cette fonction retourne un pointeur vers la fenêtre GLFW associée à l'instance actuelle.
	 *
	 * @return Un pointeur vers la fenêtre GLFW.
	 */
	[[nodiscard]] GLFWwindow* GetGlfwWindow() const { return window; }

	/**
	 * @brief Réinitialise le curseur de la fenêtre.
	 *
	 * Cette fonction réinitialise le curseur de la fenêtre à sa forme par défaut.
	 */
	void ResetCursorWindow() const { glfwSetCursor(window, nullptr); }

	/**
	 * @brief Change la forme du curseur de la fenêtre en un cube coloré.
	 *
	 * Cette fonction change la forme du curseur de la fenêtre en un cube coloré, avec la possibilité de spécifier la couleur du cube.
	 *
	 * @param _color
	 * @param color La couleur du cube. La valeur par défaut est 255.
	 */
	void CubeCursorWindow(int _color = 255) const;

	/**
	 * @brief Définit le curseur standard de la fenêtre.
	 *
	 * Cette fonction définit le curseur standard de la fenêtre à un type spécifié.
	 *
	 * @param _cursorType
	 * @param CursorType Le type de curseur à définir.
	 */
	void StandardCursorWindow(GlfwCursorType _cursorType) const;

	/**
	 * @brief Définit le mode de curseur d'entrée pour la fenêtre.
	 *
	 * Cette fonction définit le mode de curseur d'entrée pour la fenêtre à un mode spécifié.
	 *
	 * @param _mode
	 * @param mode Le mode de curseur d'entrée à définir.
	 */
	void SetInputCursorMode(GlfwCursorMode _mode) const;

	bool ShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void PollEvents() { glfwPollEvents(); }

#pragma region Getter

#pragma region Size

	[[nodiscard]] int GetWidth() const { return size.x; }
	[[nodiscard]] int GetHeight() const { return size.y; }
	[[nodiscard]] glm::vec2 GetSize() const { return size; }

	[[nodiscard]] int GetFrameBufferWidth() const { return frameBufferSize.x; }
	[[nodiscard]] int GetFrameBufferHeight() const { return frameBufferSize.y; }
	[[nodiscard]] glm::vec2 GetFrameBufferSize() const { return frameBufferSize; }

#pragma endregion

#pragma region Position

	[[nodiscard]] glm::ivec2 GetPosition() const { return position; }
	[[nodiscard]] int GetPositionX() const { return position.x; }
	[[nodiscard]] int GetPositionY() const { return position.y; }

	[[nodiscard]] glm::ivec2 GetStartingPosition() const { return startingPosition; }
	[[nodiscard]] int GetStartingPositionX() const { return startingPosition.x; }
	[[nodiscard]] int GetStartingPositionY() const { return startingPosition.y; }

#pragma endregion

	[[nodiscard]] bool GetFrameBufferResize() const { return bFrameBufferResize; }
	[[nodiscard]] bool HasFocus() const { return bHasFocus; }

	[[nodiscard]] std::string GetName() const { return windowName; }
	[[nodiscard]] std::string GetWindowTitle() const { return windowTitle; }

	[[nodiscard]] float GetAspectRatio() const { return static_cast<float>(size.x) / static_cast<float>(size.y); }
	[[nodiscard]] float GetInvAspectRatio() const { return static_cast<float>(size.y) / static_cast<float>(size.x); }
	// Autres
	[[nodiscard]] bool GetVSyncEnabled() const { return bVSyncEnabled; }
	[[nodiscard]] GlfwCursorMode GetCursorMode() const { return cursorMode; }
	[[nodiscard]] WindowMode GetWindowMode() const { return currentWindowMode; }

#pragma endregion

#pragma region Setter

#pragma region Size

	void SetSize(const glm::ivec2& _newSize) { size = _newSize; }
	void SetSize(const int32_t& _newWidth, const int32_t& _newHeight) { size = glm::ivec2(_newWidth, _newHeight); }
	void SetWidth(const int& _newWidth) { size.x = _newWidth; }
	void SetHeight(const int& _newHeight) { size.y = _newHeight; }

	void SetFrameBufferSize(const glm::vec2& _newSize) { frameBufferSize = _newSize; }

	void SetFrameBufferSize(const int32_t& _newWidth, const int32_t& _newHeight)
	{
		frameBufferSize = glm::vec2(_newWidth, _newHeight);
	}

	void SetFrameBufferWidth(const int& _newWidth) { frameBufferSize.x = _newWidth; }
	void SetFrameBufferHeight(const int& _newHeight) { frameBufferSize.y = _newHeight; }

#pragma endregion

	void SetPosition(const glm::ivec2& _position) { position = _position; }

	void SetPosition(const int32_t& _newPositionX, const int32_t& _newPositionY)
	{
		position = glm::vec2(_newPositionX, _newPositionY);
	}

	void SetPositionX(const int& _positionX) { position.x = _positionX; }
	void SetPositionY(const int& _positionY) { position.y = _positionY; }

	void SetStartingPosition(const glm::ivec2& _position) { startingPosition = _position; }

	void SetStartingPosition(const int32_t& _newPositionX, const int32_t& _newPositionY)
	{
		startingPosition = glm::vec2(_newPositionX, _newPositionY);
	}

	void SetStartingPositionX(const int& _positionX) { startingPosition.x = _positionX; }
	void SetStartingPositionY(const int& _positionY) { startingPosition.y = _positionY; }

#pragma region Position


#pragma endregion
	void SetFrameBufferResize(const bool& _state) { bFrameBufferResize = _state; }
	void SetName(const std::string& _newName) { windowName = _newName; }

	void SetWindowTitle(const std::string& _windowTitle)
	{
		windowTitle = _windowTitle;
		glfwSetWindowTitle(window, windowTitle.c_str());
	}

	void SetUpdateWindowTitleFrequency(const float _updateFrequencyInSeconds)
	{
		updateWindowTitleFrequency = _updateFrequencyInSeconds;	
	}
		
	// Autres

	void SetVSyncEnabled(const bool _bEnabled) { bVSyncEnabled = _bEnabled; }
	void SetCursorMode(const GlfwCursorMode _cursorMode) { cursorMode = _cursorMode; }

#pragma endregion

	/*void KeyCallback(Inputs::KeyCode keycode, Inputs::KeyAction action, int32_t mods){}
	void CharCallback(uint32_t character){}
	void MouseButtonCallback(Inputs::MouseButton mouseButton, Inputs::KeyAction action, int32_t mods){}
	void WindowFocusCallback(int32_t focused){}
	void CursorPosCallback(double x, double y){}
	void ScrollCallback(double xoffset, double yoffset){}
	void WindowSizeCallback(int32_t width, int32_t height, bool bMaximized, bool bIconified){}
	void WindowPosCallback(int32_t newX, int32_t newY){}
	void FrameBufferSizeCallback(int32_t width, int32_t height){}*/


	WindowMode StrToWindowMode(const char* _modeStr);

	/**
	 * @brief Initialise la fenêtre GLFW.
	 *
	 * Cette fonction initialise GLFW, configure la fenêtre pour être non redimensionnable et crée une fenêtre GLFW avec la taille et le nom spécifiés.
	 */
	void Init() override;
	void Start() override;
	void FixedUpdate() override;
	void Update() override;
	void PreRender() override;
	void Render() override;
	void RenderGui() override;
	void PostRender() override;
	void Release() override;
	void Finalize() override;

private:
	/**
		 * @brief Callback function for framebuffer resize events.
		 *
		 * This function is called when the framebuffer of the window is resized.
		 *
		 * @param _window The window that triggered the resize event.
		 * @param _width The new width of the framebuffer.
		 * @param _height The new height of the framebuffer.
		 */
	static void FrameBufferResizeCallBack(GLFWwindow* _window, int _width, int _height);

	std::string GenerateWindowTitle() const;

	bool bFrameBufferResize = false; // Booléen indiquant si le framebuffer a été redimensionné.

	SceneManager* sceneManager = nullptr;

	std::string windowName; // Nom de la fenêtre
	std::string windowTitle; // Nom de la fenêtre
	GLFWwindow* window; // Fenêtre GLFW

	glm::ivec2 size = {0, 0};
	glm::ivec2 startingPosition = {0, 0};
	glm::ivec2 position = {0, 0};
	glm::ivec2 frameBufferSize = {0, 0};
	bool bHasFocus = false;

	// Whether to move the console to an additional monitor when present
	bool bMoveConsoleToOtherMonitor = true;
	// Whether to restore the size and position from the previous session on bootup
	bool bAutoRestoreStateOnBootup = true;

	WindowMode currentWindowMode = WindowMode::_NONE;

	// Used to store previous window size and position to restore after exiting fullscreen
	glm::ivec2 lastWindowedSize;
	glm::ivec2 lastWindowedPos;
	WindowMode lastNonFullscreenWindowMode = WindowMode::_NONE;
	// Stores which mode we were in before entering fullscreen

	bool bShowFPSInWindowTitle = true;
	bool bShowMSInWindowTitle = true;
	bool bMaximized = false;
	bool bIconified = false;
	bool bVSyncEnabled = true;

	float updateWindowTitleFrequency = 0.0f;
	float secondsSinceTitleUpdate = 0.0f;

	GlfwCursorMode cursorMode = NORMAL;
};
