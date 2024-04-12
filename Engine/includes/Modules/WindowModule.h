#pragma once
#include "Module.h"

#include "lve_descriptors.h"
#include "lve_window.h"
//std
#include <string>

#include "GameObject/GameObject.h"

#define WINDOW_CONFIG_LOCATION "Saves\\window_config.json"

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
 * Cette classe repr�sente le gestionnaire de fen�tres dans le syst�me.
 * Elle h�rite de la classe Module, ce qui lui permet d'�tre int�gr�e dans le syst�me de modules.
 * Le WindowModule est responsable de la gestion et de la manipulation des fen�tres de l'application.
 */

enum GlfwCursorType
{
	ARROW,
	/**< Curseur fl�che. */
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
	NOT_ALLOWED /**< Curseur non autoris�. */
};

/**
 * @brief Enum�ration des modes de curseurs GLFW.
 *
 * Cette �num�ration repr�sente les diff�rents modes de curseurs GLFW.
 * Ces modes d�finissent le comportement du curseur sur l'�cran.
 */
enum GlfwCursorMode
{
	DISABLED,
	/**< Curseur d�sactiv�. */
	HIDDEN,
	/**< Curseur cach�. */
	NORMAL,
	/**< Curseur normal (visible). */
	CAPTURED /**< Curseur captur� (exclusif � la fen�tre). */
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
			 * @brief Enum�ration des types de curseurs GLFW.
			 *
			 * Cette �num�ration repr�sente les diff�rents types de curseurs GLFW disponibles.
			 * Ces types peuvent �tre utilis�s pour sp�cifier le style du curseur � afficher.
			 */


	static constexpr int WIDTH = 800; ///< Largeur de la fen�tre par d�faut.
	static constexpr int HEIGHT = 600; ///< Hauteur de la fen�tre par d�faut.


	/**
	 * @brief Constructeur de la classe WindowModule.
	 *
	 * Initialise une instance de WindowModule avec les dimensions sp�cifi�es et le nom de la fen�tre.
	 *
	 * @param _width La largeur de la fen�tre.
	 * @param _height La hauteur de la fen�tre.
	 * @param _name Le nom de la fen�tre.
	 * @return Rien.
	 */
	WindowModule();

	/**
	 * @brief Destructeur de la classe WindowModule.
	 *
	 * D�truit la fen�tre GLFW.
	 */
	~WindowModule();

	/**
	 * @brief Constructeur de copie supprim�.
	 *
	 * La copie d'objets de type WindowModule est explicitement interdite pour �viter les probl�mes de gestion
	 * des ressources associ�es � la fen�tre.
	 */
	WindowModule(const WindowModule&) = delete;

	/**
	 * @brief Op�rateur d'affectation par copie supprim�.
	 *
	 * L'affectation par copie d'objets de type WindowModule est explicitement interdite pour �viter les probl�mes
	 * de gestion des ressources associ�es � la fen�tre.
	 *
	 * @return Une r�f�rence vers l'objet WindowModule apr�s affectation.
	 */
	WindowModule& operator=(const WindowModule&) = delete;

	/**
	 * @brief V�rifie si la fen�tre doit �tre ferm�e.
	 *
	 * Cette fonction renvoie true si la fen�tre doit �tre ferm�e, false sinon.
	 *
	 * @return true si la fen�tre doit �tre ferm�e, false sinon.
	 */
	bool ShouldClose() const { return glfwWindowShouldClose(window); }

	/**
	 * @brief Obtient les dimensions de la fen�tre.
	 *
	 * Cette fonction retourne les dimensions de la fen�tre sous forme d'une structure vk::Extent2D.
	 *
	 * @return Les dimensions de la fen�tre sous forme d'une structure vk::Extent2D.
	 */
	vk::Extent2D GetExtent() const { return {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)}; }

	/**
	 * @brief V�rifie si la fen�tre a �t� redimensionn�e.
	 *
	 * Cette fonction v�rifie si la fen�tre a �t� redimensionn�e depuis la derni�re v�rification.
	 *
	 * @return true si la fen�tre a �t� redimensionn�e, sinon false.
	 */

	bool WasWindowResized() const { return bFrameBufferResize; }

	/**
	 * @brief R�initialise le drapeau de redimensionnement de la fen�tre.
	 *
	 * Cette fonction r�initialise le drapeau indiquant que la fen�tre a �t� redimensionn�e.
	 */
	void ResetWindowResizedFlag() { bFrameBufferResize = false; }

	/**
	 * @brief Cr�e une surface Vulkan associ�e � une fen�tre.
	 *
	 * Cette fonction cr�e une surface Vulkan associ�e � une fen�tre, permettant � Vulkan de dessiner dans cette fen�tre.
	 *
	 * @param _instance L'instance Vulkan utilis�e pour cr�er la surface.
	 * @param _surface Un pointeur vers l'objet de surface Vulkan � cr�er. Ce pointeur sera mis � jour pour contenir la surface cr��e.
	 * @throws Une exception en cas d'�chec lors de la cr�ation de la surface.
	 */
	void CreateWindowSurface(vk::Instance _instance, vk::SurfaceKHR* _surface) const;

	/**
	 * @brief Obtient un pointeur vers la fen�tre GLFW.
	 *
	 * Cette fonction retourne un pointeur vers la fen�tre GLFW associ�e � l'instance actuelle.
	 *
	 * @return Un pointeur vers la fen�tre GLFW.
	 */
	[[nodiscard]] GLFWwindow* GetGlfwWindow() const { return window; }

	/**
	 * @brief R�initialise le curseur de la fen�tre.
	 *
	 * Cette fonction r�initialise le curseur de la fen�tre � sa forme par d�faut.
	 */
	void ResetCursorWindow() const { glfwSetCursor(window, nullptr); }

	/**
	 * @brief Change la forme du curseur de la fen�tre en un cube color�.
	 *
	 * Cette fonction change la forme du curseur de la fen�tre en un cube color�, avec la possibilit� de sp�cifier la couleur du cube.
	 *
	 * @param _color
	 * @param color La couleur du cube. La valeur par d�faut est 255.
	 */
	void CubeCursorWindow(int _color = 255) const;

	/**
	 * @brief D�finit le curseur standard de la fen�tre.
	 *
	 * Cette fonction d�finit le curseur standard de la fen�tre � un type sp�cifi�.
	 *
	 * @param _cursorType
	 * @param CursorType Le type de curseur � d�finir.
	 */
	void StandardCursorWindow(GlfwCursorType _cursorType) const;

	/**
	 * @brief D�finit le mode de curseur d'entr�e pour la fen�tre.
	 *
	 * Cette fonction d�finit le mode de curseur d'entr�e pour la fen�tre � un mode sp�cifi�.
	 *
	 * @param _mode
	 * @param mode Le mode de curseur d'entr�e � d�finir.
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
	const char* WindowModeToStr(WindowMode _mode)
	{
		return WindowModeStrings[static_cast<int32_t>(_mode)];
	}

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
	 * @brief Initialise la fen�tre GLFW.
	 *
	 * Cette fonction initialise GLFW, configure la fen�tre pour �tre non redimensionnable et cr�e une fen�tre GLFW avec la taille et le nom sp�cifi�s.
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

	bool InitFromConfig();
	void SaveToConfig();

	bool bFrameBufferResize = false; // Bool�en indiquant si le framebuffer a �t� redimensionn�.

	SceneManager* sceneManager = nullptr;

	std::string windowName; // Nom de la fen�tre
	std::string windowTitle; // Nom de la fen�tre
	GLFWwindow* window; // Fen�tre GLFW

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
