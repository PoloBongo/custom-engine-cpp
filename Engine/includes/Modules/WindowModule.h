#pragma once
#include "Module.h"

#include "lve_descriptors.h"
#include "lve_window.h"
//std
#include <iostream>
#include <stb_image.h>
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
	CAPTURED,
	/**< Curseur capturé (exclusif à la fenêtre). */

	_NONE
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


		static constexpr int WIDTH  = 800; ///< Largeur de la fenêtre par défaut.
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
		void SetInputCursorMode(GlfwCursorMode _mode);

		[[nodiscard]] bool ShouldClose() const
		{
			return glfwWindowShouldClose(window);
		}

		void PollEvents() { glfwPollEvents(); }

		#pragma region Getter

		#pragma region Size

		[[nodiscard]] int       GetWidth() const { return size.x; }
		[[nodiscard]] int       GetHeight() const { return size.y; }
		[[nodiscard]] glm::vec2 GetSize() const { return size; }

		[[nodiscard]] int       GetFrameBufferWidth() const { return frameBufferSize.x; }
		[[nodiscard]] int       GetFrameBufferHeight() const { return frameBufferSize.y; }
		[[nodiscard]] glm::vec2 GetFrameBufferSize() const { return frameBufferSize; }

		#pragma endregion

		#pragma region Position

		[[nodiscard]] glm::ivec2 GetPosition() const { return position; }
		[[nodiscard]] int        GetPositionX() const { return position.x; }
		[[nodiscard]] int        GetPositionY() const { return position.y; }

		[[nodiscard]] glm::ivec2 GetStartingPosition() const { return startingPosition; }
		[[nodiscard]] int        GetStartingPositionX() const { return startingPosition.x; }
		[[nodiscard]] int        GetStartingPositionY() const { return startingPosition.y; }

		#pragma endregion

		[[nodiscard]] bool GetFrameBufferResize() const { return bFrameBufferResize; }
		[[nodiscard]] bool HasFocus() const { return bHasFocus; }

		#pragma region Name&Title

		[[nodiscard]] std::string GetName() const { return windowName; }
		[[nodiscard]] std::string GetWindowTitle() const { return windowTitle; }

		#pragma endregion

		[[nodiscard]] float GetAspectRatio() const { return static_cast<float>(size.x) / static_cast<float>(size.y); }

		[[nodiscard]] float GetInvAspectRatio() const
		{
			return static_cast<float>(size.y) / static_cast<float>(size.x);
		}

		// Autres
		[[nodiscard]] bool           GetVSyncEnabled() const { return bVSyncEnabled; }
		[[nodiscard]] GlfwCursorMode GetCursorMode() const { return cursorMode; }
		[[nodiscard]] WindowMode     GetWindowMode() const { return currentWindowMode; }

		const char* GetClipboardText() const
		{
			return glfwGetClipboardString(window);
		}

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


		#pragma region Position

		void SetPosition(const glm::ivec2& _position)
		{
			if (window) glfwSetWindowPos(window, _position.x, _position.y);
			else startingPosition = _position;
			OnPositionChanged(_position);
		}

		void SetPosition(const int32_t& _newPositionX, const int32_t& _newPositionY)
		{
			if (window) glfwSetWindowPos(window, _newPositionX, _newPositionY);
			else startingPosition = {_newPositionX, _newPositionY};
			OnPositionChanged(_newPositionX, _newPositionY);
		}

		void SetPositionX(const int& _positionX)
		{
			if (window) glfwSetWindowPos(window, _positionX, position.y);
			else startingPosition = {_positionX, position.y};
			OnPositionChanged(_positionX, position.y);
		}

		void SetPositionY(const int& _positionY)
		{
			if (window) glfwSetWindowPos(window, position.x, _positionY);
			else startingPosition = {position.x, _positionY};
			OnPositionChanged(position.x, _positionY);
		}

		void SetMousePosition(const glm::ivec2 _mousePosition) const
		{
			glfwSetCursorPos(window, _mousePosition.x, _mousePosition.y);
		}

		#pragma endregion


		#pragma region Name&Title

		void SetName(const std::string& _newName) { windowName = _newName; }

		void SetWindowTitle(const std::string& _windowTitle)
		{
			windowTitle = _windowTitle;
			glfwSetWindowTitle(window, windowTitle.c_str());
		}

		#pragma endregion

		void SetFrameBufferResize(const bool& _state) { bFrameBufferResize = _state; }

		void SetUpdateWindowTitleFrequency(const float& _updateFrequencyInSeconds)
		{
			updateWindowTitleFrequency = _updateFrequencyInSeconds;
		}

		// Autres

		void SetVSyncEnabled(const bool& _bEnabled) { bVSyncEnabled = _bEnabled; }

		void SetCursorMode(const GlfwCursorMode& _mode)
		{
			if (cursorMode != _mode) cursorMode = _mode;
				//g_InputManager->OnCursorModeChanged(_mode);
		}

		const char* WindowModeToStr()
		{
			return WindowModeStrings[static_cast<int32_t>(GetCursorMode())];
		}


		void SetWindowMode(const WindowMode& _mode, const bool _bForce = false)
		{
			if (_bForce || currentWindowMode != _mode)
			{
				currentWindowMode = _mode;

				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				if (!monitor)
				{
					std::cout << ("Failed to find primary monitor! Can't set window mode\n");
					return;
				}

				const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
				if (!video_mode)
				{
					std::cout << ("Failed to get monitor's video mode! Can't set window mode\n");
					return;
				}

				switch (_mode)
				{
					case WindowMode::FULLSCREEN:
						{
							glfwSetWindowMonitor(window, monitor, 0, 0, video_mode->width, video_mode->height,
							                     video_mode->refreshRate);
						}
						break;
					case WindowMode::WINDOWED_FULLSCREEN:
						{
							glfwSetWindowMonitor(window, monitor, 0, 0, video_mode->width, video_mode->height,
							                     video_mode->refreshRate);
							lastNonFullscreenWindowMode = WindowMode::WINDOWED_FULLSCREEN;
						}
						break;
					case WindowMode::WINDOWED:
						{
							//CHECK(lastWindowedSize.x != 0 && lastWindowedSize.y != 0);

							if (lastWindowedPos.y == 0)
								// When in windowed mode a y position of 0 means the title bar isn't
								// visible. This will occur if the app launched in fullscreen since
								// the last y position to never have been set to a valid value.
								lastWindowedPos.y = 40;

							glfwSetWindowMonitor(window, nullptr, lastWindowedPos.x, lastWindowedPos.y,
							                     lastWindowedSize.x, lastWindowedSize.y, video_mode->refreshRate);
							lastNonFullscreenWindowMode = WindowMode::WINDOWED;
						}
						break;
					case WindowMode::_NONE:
					default:
						{
							std::cout << ("Unhandled window mode: %u\n", static_cast<uint32_t>(_mode));
						}
						break;
				}
			}
		}

		void SetClipboardText(const char* _text) const
		{
			glfwSetClipboardString(window, _text);
		}

		void SetWindowIcon(const std::string& _fileName) const
		{
			int width, height, nr_channels;
			if (unsigned char* data = stbi_load(_fileName.c_str(), &width, &height, &nr_channels, 0))
			{
				GLFWimage icon;
				icon.width  = width;
				icon.height = height;
				icon.pixels = data;
				glfwSetWindowIcon(window, 1, &icon);
			}
		}

		#pragma endregion


		void OnSizeChanged(const int32_t _width, const int32_t _height)
		{
			size            = glm::ivec2(_width, _height);
			frameBufferSize = size;
			if (currentWindowMode == WindowMode::WINDOWED) lastWindowedSize = size;

			/*if (g_Renderer)
			{
				g_Renderer->OnWindowSizeChanged(_width, _height);
			}*/
		}

		void OnSizeChanged(const glm::ivec2& _newPosition)
		{
			size            = _newPosition;
			frameBufferSize = size;
			if (currentWindowMode == WindowMode::WINDOWED) lastWindowedSize = size;

			/*if (g_Renderer)
			{
				g_Renderer->OnWindowSizeChanged(_width, _height);
			}*/
		}

		void OnPositionChanged(const int32_t _newPositionX, const int32_t _newPositionY)
		{
			position = glm::ivec2{_newPositionX, _newPositionY};

			if (currentWindowMode == WindowMode::WINDOWED) lastWindowedPos = position;
		}

		void OnPositionChanged(const glm::ivec2& _newPosition)
		{
			position = _newPosition;

			if (currentWindowMode == WindowMode::WINDOWED) lastWindowedPos = position;
		}

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

		void ToggleFullscreen(const bool _bForce = false)
		{
			if (currentWindowMode == WindowMode::FULLSCREEN)
				//CHECK(lastNonFullscreenWindowMode == WindowMode::WINDOWED || lastNonFullscreenWindowMode == WindowMode::WINDOWED_FULLSCREEN);

				SetWindowMode(lastNonFullscreenWindowMode, _bForce);
			else SetWindowMode(WindowMode::FULLSCREEN, _bForce);
		}

		void Maximize() const
		{
			glfwMaximizeWindow(window);
		}

		void Iconify() const
		{
			glfwIconifyWindow(window);
		}

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

		bool InitFromConfig();
		void SaveToConfig();

		bool bFrameBufferResize = false; // Booléen indiquant si le framebuffer a été redimensionné.

		SceneManager* sceneManager = nullptr;

		std::string windowName;  // Nom de la fenêtre
		std::string windowTitle; // Nom de la fenêtre
		GLFWwindow* window;      // Fenêtre GLFW

		glm::ivec2 size             = {0, 0};
		glm::ivec2 startingPosition = {0, 0};
		glm::ivec2 position         = {0, 0};
		glm::ivec2 frameBufferSize  = {0, 0};
		bool       bHasFocus        = false;

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
		bool bShowMSInWindowTitle  = true;
		bool bMaximized            = false;
		bool bIconified            = false;
		bool bVSyncEnabled         = true;

		float updateWindowTitleFrequency = 0.0f;
		float secondsSinceTitleUpdate    = 0.0f;

		GlfwCursorMode cursorMode = NORMAL;
};
