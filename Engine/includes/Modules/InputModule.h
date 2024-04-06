#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <unordered_map>
#include <GLFW/glfw3.h>

#include "Module.h"

class InputModule final : public Module
{
	public:
		explicit InputModule(GLFWwindow* _window);
		~InputModule();

		void ProcessInput();

		bool IsKeyPressed(int _key);
		bool IsMouseButtonPressed(int _button);
		void GetMousePosition(double& _xPos, double& _yPos) const;

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
		GLFWwindow*                   window;
		std::unordered_map<int, bool> keys;
		std::unordered_map<int, bool> mouseButtons;
		double                        mouseX, mouseY;

		static void KeyCallback(GLFWwindow* _window, const int _key, const int _scancode, const int _action, const int _mods);
		static void MouseButtonCallback(GLFWwindow* _window, const int _button, const int _action, const int _mods);
		static void CursorPositionCallback(GLFWwindow* _window, const double _xPos, const double _yPos);
		static void JoystickCallback(const int _jid, const int _event);
		static void GamepadInput(const int _jid, const GLFWgamepadstate* _state);
};

#endif // INPUT_MANAGER_H
